// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphDetailsTab.h"

#include "CadenceGraphApplication.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphSchemaActions.h"
#include "CadencePalette.h"
#include "DetailLayoutBuilder.h"
#include "EditorCategoryUtils.h"
#include "GraphActionNode.h"
#include "GraphEditorDragDropAction.h"
#include "SGraphActionMenu.h"
#include "SPositiveActionButton.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/CadenceGraph.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "CadenceGraphDetails"

const FName FCadenceGraphDetailsTabFactory::Identifier = FName(TEXT("FCadenceGraphDetailsTab"));

FCadenceGraphDetailsTabFactory::FCadenceGraphDetailsTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication)
: FWorkflowTabFactory(Identifier, InApplication)
{
	Application = InApplication;

	TabLabel = FText::FromString(TEXT("Graph Details"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the details view of a Cadence Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the Graph Details view."));
}

TSharedRef<SWidget> FCadenceGraphDetailsTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FCadenceGraphApplication> App = Application.Pin();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	
	FDetailsViewArgs GraphDetailsViewArgs;
	{
		GraphDetailsViewArgs.bHideSelectionTip = true;
		GraphDetailsViewArgs.bSearchInitialKeyFocus = true;
		GraphDetailsViewArgs.bShowOptions = true;
		GraphDetailsViewArgs.NotifyHook = nullptr;
		GraphDetailsViewArgs.bAllowSearch = false;
		GraphDetailsViewArgs.bShowCustomFilterOption = false;
	}

	TSharedPtr<IDetailsView> GraphDetailsView = PropertyEditorModule.CreateDetailView(GraphDetailsViewArgs);
	GraphDetailsView->SetObject(App->GetWorkingGraph());
	App->SetGraphDetailsView(GraphDetailsView);

	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					GraphDetailsView.ToSharedRef()	
				];		
}

FText FCadenceGraphDetailsTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Cadence Graph Details View"));
}


struct FGeometry;
struct FSlateBrush;

//////////////////////////////////////////////////////////////////////////

// Magic values to differentiate Variables and Graphs on the clipboard
static const TCHAR* VAR_PREFIX = TEXT("CadenceVar");
static const TCHAR* GRAPH_PREFIX = TEXT("CadenceGraph");

//////////////////////////////////////////////////////////////////////////


void FCadenceGraphDetailsCommands::RegisterCommands() 
{	
	UI_COMMAND( OpenExternalGraph, "Open External Graph", "Opens up this external graph's graph panel in its own asset editor", EUserInterfaceActionType::Button, FInputChord() );
	UI_COMMAND( FocusNode, "Focus", "Focuses on the associated node", EUserInterfaceActionType::Button, FInputChord() );
	UI_COMMAND( DeleteEntry, "Delete", "Deletes this variable from this cadence graph.", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete), FInputChord(EKeys::BackSpace));
	UI_COMMAND( PasteVariable, "Paste Variable", "Pastes the variable to this cadence graph.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND( GotoNativeVarDefinition, "Goto Code Definition", "Goto the native code definition of this variable", EUserInterfaceActionType::Button, FInputChord() );
}

//////////////////////////////////////////////////////////////////////////

class FCadenceGraphCategoryDragDropAction : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FMyBlueprintCategoryDragDropAction, FGraphEditorDragDropAction)

	virtual void HoverTargetChanged() override
	{
		const FSlateBrush* StatusSymbol = FAppStyle::GetBrush(TEXT("NoBrush")); 
		FText Message = DraggedCategory;

		FFormatNamedArguments Args;
		Args.Add(TEXT("DraggedCategory"), DraggedCategory);

		if (!HoveredCategoryName.IsEmpty())
		{
			if(HoveredCategoryName.EqualTo(DraggedCategory))
			{
				StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));

				
				Message = FText::Format( LOCTEXT("MoveCatOverSelf", "Cannot insert category '{DraggedCategory}' before itself."), Args );
			}
			else
			{
				StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
				Args.Add(TEXT("HoveredCategory"), HoveredCategoryName);
				Message = FText::Format( LOCTEXT("MoveCatOK", "Move category '{DraggedCategory}' before '{HoveredCategory}'"), Args );
			}
		}
		else if (HoveredAction.IsValid())
		{
			StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
			Message = LOCTEXT("MoveCatOverAction", "Can only insert before another category.");
		}
		else
		{
			StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
			Message = FText::Format(LOCTEXT("MoveCatAction", "Moving category '{DraggedCategory}'"), Args);
		}

		SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, Message);
	}
	
	virtual FReply DroppedOnCategory(FText OnCategory) override
	{
		// Get MyBlueprint via MyBlueprintPtr
		TSharedPtr<SCadenceGraphDetailsTabWidget> GraphDetailsTabWidget = GraphDetailsPtr.Pin();
		if(GraphDetailsTabWidget.IsValid())
		{
			// Move the category in the blueprint category sort list
			GraphDetailsTabWidget->MoveCategoryBeforeCategory( DraggedCategory, OnCategory );
		}

		return FReply::Handled();
	}

	static TSharedRef<FCadenceGraphCategoryDragDropAction> New(const FText& InCategory, TSharedPtr<SCadenceGraphDetailsTabWidget> InGraphDetailsWidget)
	{
		TSharedRef<FCadenceGraphCategoryDragDropAction> Operation = MakeShareable(new FCadenceGraphCategoryDragDropAction);
		Operation->DraggedCategory = InCategory;
		Operation->GraphDetailsPtr = InGraphDetailsWidget;
		Operation->Construct();
		return Operation;
	}

	/** Category we were dragging */
	FText DraggedCategory;
	/** MyBlueprint widget we dragged from */
	TWeakPtr<SCadenceGraphDetailsTabWidget>	GraphDetailsPtr;
};

//////////////////////////////////////////////////////////////////////////
// FGraphActionSort

// Helper structure to aid category sorting
struct FGraphActionSort
{
public:
	FGraphActionSort(TArray<FName>& BlueprintCategorySorting)
		: bCategoriesModified(false)
		, CategorySortIndices(BlueprintCategorySorting)
	{
		CategoryUsage.Init(0, CategorySortIndices.Num());
	}

	void AddAction(const FString& Category, TSharedPtr<FEdGraphSchemaAction> Action)
	{
		// Find root category
		int32 RootCategoryDelim = Category.Find(TEXT("|"));
		FName RootCategory = RootCategoryDelim == INDEX_NONE ? *Category : *Category.Left(RootCategoryDelim);
		// Get root sort index
		const int32 SortIndex = GetSortIndex(RootCategory) + Action->GetSectionID();

		SortedActions.Add(SortIndex, Action);
	}

	void AddAction(TSharedPtr<FEdGraphSchemaAction> Action)
	{
		const FString UserCategoryName = FEditorCategoryUtils::GetCategoryDisplayString(Action->GetCategory().ToString());
		AddAction(UserCategoryName, Action);
	}

	void GetAllActions(FGraphActionListBuilderBase& OutActions)
	{
		SortedActions.KeySort(TLess<int32>());

		for (const auto& Iter : SortedActions)
		{
			OutActions.AddAction(Iter.Value);
		}
	}

	void CleanupCategories()
	{
		// Scrub unused categories from the blueprint
		if (bCategoriesModified)
		{
			for (int32 CategoryIdx = CategoryUsage.Num() - 1; CategoryIdx >= 0; CategoryIdx--)
			{
				if (CategoryUsage[CategoryIdx] == 0)
				{
					CategorySortIndices.RemoveAt(CategoryIdx);
				}
			}
			bCategoriesModified = false;
		}
	}

private:
	const int32 GetSortIndex(FName Category)
	{
		int32 SortIndex = CategorySortIndices.Find(Category);

		if (SortIndex == INDEX_NONE)
		{
			bCategoriesModified = true;
			SortIndex = CategorySortIndices.Add(Category);
			CategoryUsage.Add(0);
		}
		CategoryUsage[SortIndex]++;
		// Spread the sort values so we can fine tune sorting
		SortIndex *= 1000;

		return SortIndex + SortedActions.Num();
	}

private:
	/** Signals if the blueprint categories have been modified and require cleanup */
	bool bCategoriesModified;
	/** Tracks category usage to aid removal of unused categories */
	TArray<int32> CategoryUsage;
	/** Reference to the category sorting in the blueprint */
	TArray<FName>& CategorySortIndices;
	/** Map used to sort Graph actions */
	TMultiMap<int32, TSharedPtr<FEdGraphSchemaAction>> SortedActions;
};

//////////////////////////////////////////////////////////////////////////

void SCadenceGraphDetailsTabWidget::Construct(const FArguments& InArgs, TWeakPtr<FCadenceGraphApplication> InGraphApplication)
{
	bNeedsRefresh = false;

	ApplicationPtr = InGraphApplication;
	EdGraph = nullptr;
	
	TSharedPtr<SWidget> ToolbarBuilderWidget = TSharedPtr<SWidget>();

	if( ApplicationPtr.IsValid() )
	{
		CadenceGraph = ApplicationPtr.Pin()->GetWorkingGraph();

		CommandList = MakeShareable(new FUICommandList);
		
		CommandList->Append(ApplicationPtr.Pin()->GetToolkitCommands());

		CommandList->MapAction( FCadenceGraphDetailsCommands::Get().OpenExternalGraph,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnOpenExternalGraph),
			FCanExecuteAction(), FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanOpenExternalGraph) );
		
		CommandList->MapAction( FCadenceGraphDetailsCommands::Get().FocusNode,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnFocusNode),
			FCanExecuteAction(), FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanFocusOnNode) );
			
		CommandList->MapAction( FCadenceGraphDetailsCommands::Get().DeleteEntry,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnDeleteEntry),
			FCanExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanDeleteEntry) );

		CommandList->MapAction( FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnDuplicateAction),
			FCanExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanDuplicateAction),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &SCadenceGraphDetailsTabWidget::IsDuplicateActionVisible) );

		ToolbarBuilderWidget = SNullWidget::NullWidget;
	
		CommandList->MapAction(FGenericCommands::Get().Rename,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnRequestRenameOnActionNode),
			FCanExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanRequestRenameOnActionNode));

		CommandList->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnCopy),
			FCanExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanCopy));
		
		CommandList->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnCut),
			FCanExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanCut));

		CommandList->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnPasteGeneric),
			FCanExecuteAction(), FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanPasteGeneric));

		CommandList->MapAction(FCadenceGraphDetailsCommands::Get().PasteVariable,
			FExecuteAction::CreateSP(this, &SCadenceGraphDetailsTabWidget::OnPasteVariable),
			FCanExecuteAction(), FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &SCadenceGraphDetailsTabWidget::CanPasteVariable));
	}

	TSharedPtr<SWidget> AddNewMenu = SNew(SPositiveActionButton)
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CadenceAddNewCombo")))
		.Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
		.Text(LOCTEXT("AddNewLabel", "Add"))
		.ToolTipText(LOCTEXT("AddNewToolTip", "Add a new Variable."))
		.IsEnabled(this, &SCadenceGraphDetailsTabWidget::IsEditingMode)
		.OnGetMenuContent(this, &SCadenceGraphDetailsTabWidget::CreateAddNewMenuWidget);

	FMenuBuilder ViewOptions(true, nullptr);	

	ViewOptions.AddMenuEntry(
		LOCTEXT("ShowEmptySections", "Show Empty Sections"),
		LOCTEXT("ShowEmptySectionsTooltip", "Should we show empty sections?"),
		FSlateIcon(),
		FUIAction( 
			FExecuteAction::CreateSP( this, &SCadenceGraphDetailsTabWidget::OnToggleShowEmptySections ),
			FCanExecuteAction(),
			FIsActionChecked::CreateSP(this, &SCadenceGraphDetailsTabWidget::IsShowingEmptySections)
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton,
		TEXT("CadenceGraphDetails_ShowEmptySections")
	);

	SAssignNew(FilterBox, SSearchBox)
		.OnTextChanged( this, &SCadenceGraphDetailsTabWidget::OnFilterTextChanged );

	// create the main action list piece of this widget
	SAssignNew(GraphActionMenu, SGraphActionMenu, false)
		.OnGetFilterText(this, &SCadenceGraphDetailsTabWidget::GetFilterText)
		.OnCreateWidgetForAction(this, &SCadenceGraphDetailsTabWidget::OnCreateWidgetForAction)
		.OnCollectAllActions(this, &SCadenceGraphDetailsTabWidget::CollectAllActions)
		.OnCollectStaticSections(this, &SCadenceGraphDetailsTabWidget::CollectStaticSections)
		.OnActionDragged(this, &SCadenceGraphDetailsTabWidget::OnActionDragged)
		.OnCategoryDragged(this, &SCadenceGraphDetailsTabWidget::OnCategoryDragged)
		.OnActionSelected(this, &SCadenceGraphDetailsTabWidget::OnGlobalActionSelected)
		.OnActionDoubleClicked(this, &SCadenceGraphDetailsTabWidget::OnActionDoubleClicked)
		.OnContextMenuOpening(this, &SCadenceGraphDetailsTabWidget::OnContextMenuOpening)
		.OnCategoryTextCommitted(this, &SCadenceGraphDetailsTabWidget::OnCategoryNameCommitted)
		.OnCanRenameSelectedAction(this, &SCadenceGraphDetailsTabWidget::CanRequestRenameOnActionNode)
		.OnGetSectionTitle(this, &SCadenceGraphDetailsTabWidget::OnGetSectionTitle)
		.OnGetSectionWidget(this, &SCadenceGraphDetailsTabWidget::OnGetSectionWidget)
		.OnActionMatchesName(this, &SCadenceGraphDetailsTabWidget::HandleActionMatchesName)
		.DefaultRowExpanderBaseIndentLevel(1)
		.AlphaSortItems(false)
		.UseSectionStyling(true);


	// now piece together all the content for this widget
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(4.0f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("MyBlueprintPanel")))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ToolbarBuilderWidget.ToSharedRef()
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 4, 0)
					[
						AddNewMenu.ToSharedRef()
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						FilterBox.ToSharedRef()
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0, 0, 0)
					[
						SNew(SComboButton)
						.ContentPadding(0.0f)
						.ComboButtonStyle(&FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>("SimpleComboButton"))
						.HasDownArrow(false)
						.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
						.ButtonContent()
						[
							SNew(SImage)
							.ColorAndOpacity(FSlateColor::UseForeground())
							.Image(FAppStyle::Get().GetBrush("Icons.Settings"))
						]
						.MenuContent()
						[
							ViewOptions.MakeWidget()
						]
					]
				]
			]
		]
		
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			GraphActionMenu.ToSharedRef()
		]
	];
	
	ResetLastPinType();

	TMap<int32, bool> ExpandedSections;
	ExpandedSections.Add(CadenceNodeSectionID::VARIABLE, true);
	ExpandedSections.Add(CadenceNodeSectionID::GRAPH, true);

	GraphActionMenu->SetSectionExpansion(ExpandedSections);

	FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &SCadenceGraphDetailsTabWidget::OnObjectPropertyChanged);
}

SCadenceGraphDetailsTabWidget::~SCadenceGraphDetailsTabWidget()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

void SCadenceGraphDetailsTabWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if(bNeedsRefresh)
	{
		Refresh();
	}
}

FReply SCadenceGraphDetailsTabWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (CommandList.IsValid() && CommandList->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SCadenceGraphDetailsTabWidget::OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< FGraphActionNode > InAction )
{
	// Remove excess whitespace and prevent categories with just spaces
	FText CategoryName = FText::TrimPrecedingAndTrailing(InNewText);

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
	GraphActionMenu->GetCategorySubActions(InAction, Actions);

	if (Actions.Num())
	{
		const FScopedTransaction Transaction( LOCTEXT( "RenameCategory", "Rename Category" ) );

		GetMainGraph()->Modify();

		for (int32 i = 0; i < Actions.Num(); ++i)
		{
			if (Actions[i]->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
			{
				FCadenceVariableAction* VarAction = (FCadenceVariableAction*)Actions[i].Get();

				if(UCadenceVariable* Var = VarAction->GetVariable())
				{
					Var->SetCategory(CategoryName);
				}
			}			
		}
		Refresh();
		ApplicationPtr.Pin()->GetWorkingAsset()->Modify();
		SelectItemByName(FName(*CategoryName.ToString()), ESelectInfo::OnMouseClick, InAction.Pin()->SectionID, true);
	}
}

FText SCadenceGraphDetailsTabWidget::OnGetSectionTitle( int32 InSectionID )
{
	FText SeparatorTitle;
	/* Setup an appropriate name for the section for this node */
	switch( InSectionID )
	{
	case CadenceNodeSectionID::VARIABLE:
		SeparatorTitle = NSLOCTEXT("GraphActionNode", "Variables", "Variables");
		break;
	case CadenceNodeSectionID::GRAPH:
		SeparatorTitle = NSLOCTEXT("GraphActionNode", "Graphs", "Graphs");
		break;
	default:
	case CadenceNodeSectionID::NONE:
		SeparatorTitle = FText::GetEmpty();
		break;
	}
	return SeparatorTitle;
}

TSharedRef<SWidget> SCadenceGraphDetailsTabWidget::OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID)
{
	TWeakPtr<SWidget> WeakRowWidget = RowWidget;

	FText AddNewText;
	FName MetaDataTag;

	switch ( InSectionID )
	{
	case CadenceNodeSectionID::VARIABLE:
		AddNewText = LOCTEXT("AddNewVariable", "Variable");
		MetaDataTag = TEXT("AddNewVariable");
		break;
	default:
		return SNullWidget::NullWidget;
	}

	return CreateAddToSectionButton(InSectionID, WeakRowWidget, AddNewText, MetaDataTag);
}

TSharedRef<SWidget> SCadenceGraphDetailsTabWidget::CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag)
{
	return 
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.OnClicked(this, &SCadenceGraphDetailsTabWidget::OnAddButtonClickedOnSection, InSectionID)
		.IsEnabled(this, &SCadenceGraphDetailsTabWidget::CanAddNewElementToSection, InSectionID)
		.ContentPadding(FMargin(1, 0))
		.AddMetaData<FTagMetaData>(FTagMetaData(MetaDataTag))
		.ToolTipText(AddNewText)
		[
			SNew(SImage)
			.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
			.ColorAndOpacity(FSlateColor::UseForeground())
		];
}

FReply SCadenceGraphDetailsTabWidget::OnAddButtonClickedOnSection(int32 InSectionID)
{
	switch ( InSectionID )
	{
	case CadenceNodeSectionID::VARIABLE:
		CommandList->ExecuteAction(FCadenceGraphDetailsCommands::Get().AddNewVariable.ToSharedRef());
		break;
	}

	return FReply::Handled();
}

bool SCadenceGraphDetailsTabWidget::CanAddNewElementToSection(int32 InSectionID) const
{
	if (!IsEditingMode())
	{
		return false;
	}

	switch (InSectionID)
	{
	case CadenceNodeSectionID::VARIABLE:
		return true;
	}	

	return false;
}

bool SCadenceGraphDetailsTabWidget::HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const
{
	return false;
}

EVisibility SCadenceGraphDetailsTabWidget::OnGetSectionTextVisibility(TWeakPtr<SWidget> RowWidget, int32 InSectionID) const
{
	// If the row is currently hovered, or a menu is being displayed for a button, keep the button expanded.
	if ( RowWidget.Pin()->IsHovered() )
	{
		return EVisibility::SelfHitTestInvisible;
	}
	else
	{
		return EVisibility::Hidden;
	}
}

bool SCadenceGraphDetailsTabWidget::CanRequestRenameOnActionNode(TWeakPtr<FGraphActionNode> InSelectedNode) const
{
	bool bIsReadOnly = true;

	// If checking if renaming is available on a category node, the category must have a non-native entry
	if (InSelectedNode.Pin()->IsCategoryNode())
	{
		TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
		GraphActionMenu->GetCategorySubActions(InSelectedNode, Actions);

		for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
		{
			if (Action->GetPersistentItemDefiningObject().IsPotentiallyEditable())
			{
				bIsReadOnly = false;
				break;
			}
		}
	}
	
	return IsEditingMode() && !bIsReadOnly;
}

void SCadenceGraphDetailsTabWidget::Refresh()
{
	bNeedsRefresh = false;

	GraphActionMenu->RefreshAllActions(/*bPreserveExpansion=*/ true);
}

TSharedRef<SWidget> SCadenceGraphDetailsTabWidget::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(SCadencePaletteItem, InCreateData, ApplicationPtr.Pin());
}

void SCadenceGraphDetailsTabWidget::GetChildGraphs(UEdGraph* InEdGraph, int32 const SectionId, FGraphActionSort& SortList, const FText& ParentCategory) const
{
	check(InEdGraph);

	// Grab display info
	FGraphDisplayInfo EdGraphDisplayInfo;
	if (const UEdGraphSchema* Schema = InEdGraph->GetSchema())
	{
		Schema->GetGraphDisplayInformation(*InEdGraph, EdGraphDisplayInfo);
	}
	const FText EdGraphDisplayName = EdGraphDisplayInfo.DisplayName;

	// Grab children graphs
	for (UEdGraph* Graph : InEdGraph->SubGraphs)
	{
		if (Graph == nullptr)
		{
			ensureMsgf(Graph != nullptr, TEXT("A subgraph of %s was null"), *GetPathNameSafe(InEdGraph));
			continue;
		}

		FGraphDisplayInfo ChildGraphDisplayInfo;
		if (const UEdGraphSchema* ChildSchema = Graph->GetSchema())
		{
			ChildSchema->GetGraphDisplayInformation(*Graph, ChildGraphDisplayInfo);
		}

		FText DisplayText = ChildGraphDisplayInfo.DisplayName;

		FText Category;
		if (!ParentCategory.IsEmpty())
		{
			Category = FText::Format(FText::FromString(TEXT("{0}|{1}")), ParentCategory, EdGraphDisplayName);
		}
		else
		{
			Category = EdGraphDisplayName;
		}

		const FName DisplayName = FName(*DisplayText.ToString());
		FText ChildTooltip = DisplayText;
		FText ChildDesc = MoveTemp(DisplayText);

		TSharedPtr<FCadenceGraphAction> NewChildAction = MakeShareable(new FCadenceGraphAction(ECadenceGraphAction::Subgraph, Category, MoveTemp(ChildDesc), MoveTemp(ChildTooltip), 1, SectionId));
		NewChildAction->FuncName = DisplayName;
		NewChildAction->EdGraph = Graph;
		SortList.AddAction(NewChildAction);
		
		GetChildGraphs(Graph, SectionId, SortList, Category);
	}
}

void SCadenceGraphDetailsTabWidget::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	UCadenceGraph* CadenceGraph = GetMainGraph();
	check(CadenceGraph);

	TSharedPtr<FCadenceGraphApplication> CadenceApplication = ApplicationPtr.Pin();

	FGraphActionSort SortList( CadenceGraph->CategorySorting );

	for(FCadenceNamedVariable& NamedVar : CadenceGraph->UserVariables.Variables)
	{
		UCadenceVariable* Var = NamedVar.Variable;

		const FString UserCategoryName = Var->GetCategory().ToString();
		TSharedPtr<FCadenceVariableAction> NewVarAction = MakeShareable(new FCadenceVariableAction());
		SortList.AddAction( UserCategoryName, NewVarAction );
	}

	// Add all the sorted variables, components, functions, etc...
	SortList.CleanupCategories();
	SortList.GetAllActions(OutAllActions);
}

void SCadenceGraphDetailsTabWidget::CollectStaticSections(TArray<int32>& StaticSectionIDs)
{
	TSharedPtr<FCadenceGraphApplication> CadenceGraphApplication = ApplicationPtr.Pin();
	const bool bIsEditor = CadenceGraphApplication.IsValid();

	if ( IsShowingEmptySections() )
	{
		if (!bIsEditor || CadenceGraphApplication->IsSectionVisible(CadenceNodeSectionID::GRAPH))
		{
			StaticSectionIDs.Add(CadenceNodeSectionID::GRAPH);
		}
		if (!bIsEditor || CadenceGraphApplication->IsSectionVisible(CadenceNodeSectionID::VARIABLE))
		{
			StaticSectionIDs.Add(CadenceNodeSectionID::VARIABLE);
		}
	}
}

void SCadenceGraphDetailsTabWidget::OnToggleShowEmptySections()
{
	bIsShowingEmptySections = !bIsShowingEmptySections;
	Refresh();
}

bool SCadenceGraphDetailsTabWidget::IsShowingEmptySections() const
{
	return bIsShowingEmptySections;
}

FReply SCadenceGraphDetailsTabWidget::OnActionDragged( const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent )
{
	if (!ApplicationPtr.IsValid())
	{
		return FReply::Unhandled();
	}

	TSharedPtr<FEdGraphSchemaAction> InAction( InActions.Num() > 0 ? InActions[0] : nullptr );
	if(InAction.IsValid())
	{		
		if(InAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
		{
			FCadenceVariableAction* VarAction = (FCadenceVariableAction*)InAction.Get();
			if (UCadenceVariable* Vars = VarAction->GetVariable())
			{
				// TODO: Support drag drop variables
				/*
				TSharedRef<FKismetVariableDragDropAction> DragOperation = FKismetVariableDragDropAction::New(InAction, VarAction->GetVariableName(), VarClass, AnalyticsDelegate);
				DragOperation->SetAltDrag(MouseEvent.IsAltDown());
				DragOperation->SetCtrlDrag(MouseEvent.IsLeftControlDown() || MouseEvent.IsRightControlDown());
				return FReply::Handled().BeginDragDrop(DragOperation);
				*/
			}
		}
	}

	return FReply::Unhandled();
}

FReply SCadenceGraphDetailsTabWidget::OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent)
{
	TSharedRef<FCadenceGraphCategoryDragDropAction> DragOperation = FCadenceGraphCategoryDragDropAction::New(InCategory, SharedThis(this));
	return FReply::Handled().BeginDragDrop(DragOperation);
}

void SCadenceGraphDetailsTabWidget::OnGlobalActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType)
{
	if (InSelectionType == ESelectInfo::OnMouseClick  || InSelectionType == ESelectInfo::OnKeyPress || InSelectionType == ESelectInfo::OnNavigation || InActions.Num() == 0)
	{
		OnActionSelected(InActions);
	}
}

void SCadenceGraphDetailsTabWidget::OnActionSelected( const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions )
{
	TSharedPtr<FEdGraphSchemaAction> InAction(InActions.Num() > 0 ? InActions[0] : NULL);
	OnActionSelectedHelper(InAction);
}

void SCadenceGraphDetailsTabWidget::OnActionSelectedHelper(TSharedPtr<FEdGraphSchemaAction> InAction)
{
	TSharedPtr<FCadenceGraphApplication> GraphApplication = ApplicationPtr.Pin();
	
	if (GraphApplication.IsValid() && InAction.IsValid())
	{
		if (InAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
		{
			FCadenceVariableAction* VarAction = (FCadenceVariableAction*)InAction.Get();
			UCadenceVariable* Var = VarAction->GetVariable();
			
			GraphApplication->GetSelectedDetailsView()->SetObject(Var);
		}
		else if (InAction->GetTypeId() == FCadenceGraphAction::StaticGetTypeId())
		{
			FCadenceGraphAction* GraphAction = (FCadenceGraphAction*)InAction.Get();
		}
	}
}

void SCadenceGraphDetailsTabWidget::OnActionDoubleClicked(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions)
{
	if ( !ApplicationPtr.IsValid() )
	{
		return;
	}

	TSharedPtr<FEdGraphSchemaAction> InAction(InActions.Num() > 0 ? InActions[0] : NULL);
	ExecuteAction(InAction);
}

void SCadenceGraphDetailsTabWidget::ExecuteAction(TSharedPtr<FEdGraphSchemaAction> InAction)
{
	if(InAction.IsValid())
	{		
		InAction->OnDoubleClick(nullptr);		
	}
}

template<class SchemaActionType> SchemaActionType* SelectionAsType( const TSharedPtr< SGraphActionMenu >& GraphActionMenu )
{
	TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	SchemaActionType* Selection = NULL;

	TSharedPtr<FEdGraphSchemaAction> SelectedAction( SelectedActions.Num() > 0 ? SelectedActions[0] : NULL );
	if ( SelectedAction.IsValid() &&
		 SelectedAction->GetTypeId() == SchemaActionType::StaticGetTypeId() )
	{
		// TODO Why not? StaticCastSharedPtr<>()

		Selection = (SchemaActionType*)SelectedActions[0].Get();
	}

	return Selection;
}

FCadenceGraphAction* SCadenceGraphDetailsTabWidget::SelectionAsGraph() const
{
	return SelectionAsType<FCadenceGraphAction>( GraphActionMenu );
}

FCadenceVariableAction* SCadenceGraphDetailsTabWidget::SelectionAsVar() const
{
	return SelectionAsType<FCadenceVariableAction>( GraphActionMenu );
}

bool SCadenceGraphDetailsTabWidget::SelectionIsCategory() const
{
	return !SelectionHasContextMenu();
}

bool SCadenceGraphDetailsTabWidget::SelectionHasContextMenu() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);
	return SelectedActions.Num() > 0;
}

FText SCadenceGraphDetailsTabWidget::GetGraphCategory(UEdGraph* InGraph) const
{
	return FText::GetEmpty();
}

void SCadenceGraphDetailsTabWidget::GetSelectedItemsForContextMenu(TArray<FComponentEventConstructionData>& OutSelectedItems) const
{
	FEdGraphSchemaAction_K2Var* Var = SelectionAsVar();
	if ( Var != NULL )
	{
		FObjectProperty* ComponentProperty = CastField<FObjectProperty>(Var->GetProperty());

		if ( ComponentProperty != NULL &&
			 ComponentProperty->PropertyClass != NULL &&
			 ComponentProperty->PropertyClass->IsChildOf( UActorComponent::StaticClass() ) )
		{
			FComponentEventConstructionData NewItem;
			NewItem.VariableName = Var->GetVariableName();
			NewItem.Component = Cast<UActorComponent>(ComponentProperty->PropertyClass->GetDefaultObject());

			OutSelectedItems.Add( NewItem );
		}
	}
}

TSharedPtr<SWidget> SCadenceGraphDetailsTabWidget::OnContextMenuOpening()
{
	if( !BlueprintEditorPtr.IsValid() )
	{
		return TSharedPtr<SWidget>();
	}

	const bool bShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder( bShouldCloseWindowAfterMenuSelection, CommandList);
	
	// Check if the selected action is valid for a context menu
	if (SelectionHasContextMenu())
	{
		FEdGraphSchemaAction_K2Var* Var = SelectionAsVar();
		FEdGraphSchemaAction_K2Graph* Graph = SelectionAsGraph();
		FEdGraphSchemaAction_K2Event* Event = SelectionAsEvent();
		const bool bExpandFindReferences = Graph || Event || Var;
		
		MenuBuilder.BeginSection("BasicOperations");
		{
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().OpenGraph);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().OpenGraphInNewTab);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().OpenExternalGraph);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().FocusNode);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().FocusNodeInNewTab);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"), LOCTEXT("Rename_Tooltip", "Renames this function or variable from blueprint.") );
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().ImplementFunction);

			// Depending on context, FindReferences can be a button or an expandable menu. For example, the context menu
			// for functions now lets you choose whether to do search by-name (fast) or by-function (smart).
			if (!bExpandFindReferences)
			{
				// No expandable menu: display the simple 'Find References' action
				MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
			}
			else
			{
				// Insert "Find References" sub-menu here
				MenuBuilder.AddSubMenu(
					LOCTEXT("FindReferences_Label", "Find References"),
					LOCTEXT("FindReferences_Tooltip", "Options for finding references to class members"),
					FNewMenuDelegate::CreateStatic(&FGraphEditorCommands::BuildFindReferencesMenu),
					false,
					FSlateIcon()
				);
			}
			
			MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindAndReplaceReferences);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().GotoNativeVarDefinition);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().MoveVariableToParent);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().MoveFunctionToParent);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().DeleteEntry);
		}
		MenuBuilder.EndSection();

		if ( Var && BlueprintEditorPtr.IsValid() && FBlueprintEditorUtils::DoesSupportEventGraphs(GetBlueprintObj()) )
		{
			FObjectProperty* ComponentProperty = CastField<FObjectProperty>(Var->GetProperty());

			if ( ComponentProperty && ComponentProperty->PropertyClass &&
				 ComponentProperty->PropertyClass->IsChildOf( UActorComponent::StaticClass() ) )
			{
				if( FBlueprintEditorUtils::CanClassGenerateEvents( ComponentProperty->PropertyClass ))
				{
					TSharedPtr<FBlueprintEditor> BlueprintEditor(BlueprintEditorPtr.Pin());

					// If the selected item is valid, and is a component of some sort, build a context menu
					// of events appropriate to the component.
					MenuBuilder.AddSubMenu(	LOCTEXT("AddEventSubMenu", "Add Event"), 
											LOCTEXT("AddEventSubMenu_ToolTip", "Add Event"), 
											FNewMenuDelegate::CreateStatic(	&SSubobjectBlueprintEditor::BuildMenuEventsSection,
																											BlueprintEditor->GetBlueprintObj(), ComponentProperty->PropertyClass.Get(), 
												FCanExecuteAction::CreateRaw(this, &SCadenceGraphDetailsTabWidget::IsEditingMode),
												FGetSelectedObjectsDelegate::CreateSP(this, &SCadenceGraphDetailsTabWidget::GetSelectedItemsForContextMenu)));
				}
			}
		}
		// If this is a function graph than we should add the option to convert it to an event if possible
		else if( Graph && Graph->EdGraph )
		{
			// The first function entry node will have all the information that the conversion needs
			// (the interface method entry in the tree might not have a real graph though, if it comes from a parent unchanged or is an event that hasn't been implemented yet)
			UK2Node_FunctionEntry* EntryNode = nullptr;
			if (Graph->EdGraph != nullptr)
			{
				for( UEdGraphNode* Node : Graph->EdGraph->Nodes)
				{
					if (UK2Node_FunctionEntry* TypedNode = Cast<UK2Node_FunctionEntry>(Node))
					{
						EntryNode = TypedNode;
						break;
					}
				}
			}

			TSharedPtr<FBlueprintEditor> BlueprintEditor(BlueprintEditorPtr.Pin());
			if( EntryNode && BlueprintEditor.IsValid() &&
				FBlueprintEditorUtils::IsFunctionConvertableToEvent(BlueprintEditor->GetBlueprintObj(), EntryNode->FindSignatureFunction()) )
			{
				MenuBuilder.AddMenuEntry(
					LOCTEXT("CadenceGraphDetails_Conversion_Func", "Convert function to event"), FText(), FSlateIcon(),
					FExecuteAction::CreateLambda([BlueprintEditor, EntryNode]()
					{
						// ConvertFunctionIfValid handles any bad state, so no need for additional messaging
						BlueprintEditor->ConvertFunctionIfValid(EntryNode);
					})
				);
			}
		}
		// If this is an event, allow us to convert it to a function graph if possible
		else if( Event )
		{
			TSharedPtr<FBlueprintEditor> BlueprintEditor(BlueprintEditorPtr.Pin());			
			UK2Node_Event* EventNode = Cast<UK2Node_Event>(Event->NodeTemplate);
			
			if( BlueprintEditor.IsValid() && EventNode )
			{
				MenuBuilder.AddMenuEntry(
					LOCTEXT("CadenceGraphDetails_Conversion_Event", "Convert event to function"), FText(), FSlateIcon(),
					FExecuteAction::CreateLambda([BlueprintEditor, EventNode]()
					{
						// The ConvertEventIfValid function handles all bad states, so there's no need for further validation
						BlueprintEditor->ConvertEventIfValid(EventNode);
					})
				);
			}			
		}
	}
	else
	{
		BuildAddNewMenu(MenuBuilder);
	}

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> SCadenceGraphDetailsTabWidget::CreateAddNewMenuWidget()
{
	const bool bShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, CommandList);

	BuildAddNewMenu(MenuBuilder);

	return MenuBuilder.MakeWidget();
}

void SCadenceGraphDetailsTabWidget::BuildAddNewMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("AddNewItem", LOCTEXT("AddOperations", "Add New"));

	if(UBlueprint* CurrentBlueprint = GetBlueprintObj())
	{
		if (CurrentBlueprint->SupportsGlobalVariables())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewVariable);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().PasteVariable);
		}
		if (CurrentBlueprint->SupportsLocalVariables())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewLocalVariable);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().PasteLocalVariable);
		}
		if (CurrentBlueprint->SupportsFunctions())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewFunction);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().PasteFunction);

			// If we cannot handle Function Graphs, we cannot handle function overrides
			if (OverridableFunctionActions.Num() > 0 && BlueprintEditorPtr.Pin()->NewDocument_IsVisibleForType(FBlueprintEditor::CGT_NewFunctionGraph))
			{
				MenuBuilder.AddSubMenu(
					LOCTEXT("OverrideFunction", "Override Function"),
					FText::GetEmpty(),
					FNewMenuDelegate::CreateSP(this, &SCadenceGraphDetailsTabWidget::BuildOverridableFunctionsMenu),
					false,
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewFunction.Small"));
			}
		}

		if (CurrentBlueprint->SupportsMacros())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewMacroDeclaration);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().PasteMacro);
		}
		if (CurrentBlueprint->SupportsEventGraphs())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewEventGraph);
		}
		if (CurrentBlueprint->SupportsDelegates())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewDelegate);
		}
		if (CurrentBlueprint->SupportsAnimLayers())
		{
			MenuBuilder.AddMenuEntry(FBlueprintEditorCommands::Get().AddNewAnimationLayer);
		}
	}
	MenuBuilder.EndSection();
}


bool SCadenceGraphDetailsTabWidget::CanOpenExternalGraph() const 
{
	const FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph();
	const bool bGraph = GraphAction && GraphAction->EdGraph;
	return CanOpenGraph() && bGraph && !BlueprintEditorPtr.Pin()->IsGraphInCurrentBlueprint(GraphAction->EdGraph);
}

void SCadenceGraphDetailsTabWidget::OpenGraph(FDocumentTracker::EOpenDocumentCause InCause, bool bOpenExternalGraphInNewEditor)
{
	UEdGraph* GraphToOpen = nullptr;

	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		GraphToOpen = GraphAction->EdGraph;
		// If we have no graph then this is an interface event, so focus on the event graph
		if (!GraphToOpen)
		{
			GraphToOpen = FBlueprintEditorUtils::FindEventGraph(GetBlueprintObj());
		}
	}
	else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
	{
		GraphToOpen = DelegateAction->EdGraph;
	}
	else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
	{
		GraphToOpen = EventAction->NodeTemplate->GetGraph();
	}
	else if (FEdGraphSchemaAction_K2InputAction* InputAction = SelectionAsInputAction())
	{
		GraphToOpen = InputAction->NodeTemplate->GetGraph();
	}
	
	if (GraphToOpen)
	{
		if(bOpenExternalGraphInNewEditor && !BlueprintEditorPtr.Pin()->IsGraphInCurrentBlueprint(GraphToOpen))
		{
			if(UBlueprint* OtherBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(GraphToOpen))
			{
				GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(OtherBlueprint);
				if(IBlueprintEditor* OtherBlueprintEditor = static_cast<IBlueprintEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(OtherBlueprint, true)))
				{
					OtherBlueprintEditor->JumpToHyperlink(GraphToOpen, false);
				}
			}
		}
		else
		{
			BlueprintEditorPtr.Pin()->OpenDocument(GraphToOpen, InCause);
		}
	}
}


void SCadenceGraphDetailsTabWidget::OnOpenExternalGraph()
{
	OpenGraph(FDocumentTracker::OpenNewDocument, true);
}

bool SCadenceGraphDetailsTabWidget::CanFocusOnNode() const
{
	FEdGraphSchemaAction_K2Event const* const EventAction = SelectionAsEvent();
	FEdGraphSchemaAction_K2InputAction const* const InputAction = SelectionAsInputAction();
	UK2Node_Event* ExistingNode = nullptr;

	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Is this an event implemented from an interface?
		UBlueprint* BlueprintObj = GetBlueprintObj();		
		UFunction* OverrideFunc = nullptr;
		UClass* const OverrideFuncClass = FBlueprintEditorUtils::GetOverrideFunctionClass(BlueprintObj, GraphAction->FuncName, &OverrideFunc);

		if (OverrideFunc)
		{
			// Add to event graph
			FName EventName = OverrideFunc->GetFName();
			ExistingNode = FBlueprintEditorUtils::FindOverrideForFunction(BlueprintObj, OverrideFuncClass, EventName);
		}
	}

	return (EventAction && EventAction->NodeTemplate) || (InputAction && InputAction->NodeTemplate) || ExistingNode;
}

void SCadenceGraphDetailsTabWidget::OnFocusNode()
{
	FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent();
	FEdGraphSchemaAction_K2InputAction* InputAction = SelectionAsInputAction();
	if (EventAction || InputAction)
	{
		UK2Node* Node = EventAction ? EventAction->NodeTemplate : InputAction->NodeTemplate;
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Node);
	}
	else if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Is this an event implemented from an interface?
		UBlueprint* BlueprintObj = GetBlueprintObj();
		UFunction* OverrideFunc = nullptr;
		UClass* const OverrideFuncClass = FBlueprintEditorUtils::GetOverrideFunctionClass(BlueprintObj, GraphAction->FuncName, &OverrideFunc);

		if (OverrideFunc)
		{
			// Add to event graph
			FName EventName = OverrideFunc->GetFName();
			if (UK2Node_Event* ExistingNode = FBlueprintEditorUtils::FindOverrideForFunction(BlueprintObj, OverrideFuncClass, EventName))
			{
				FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
			}
		}
	}
}

void SCadenceGraphDetailsTabWidget::OnDeleteGraph(UEdGraph* InGraph, EEdGraphSchemaAction_K2Graph::Type InGraphType)
{
	if (InGraph && InGraph->bAllowDeletion)
	{
		if (const UEdGraphSchema* Schema = InGraph->GetSchema())
		{
			if (Schema->TryDeleteGraph(InGraph))
			{
				return;
			}
		}

		const FScopedTransaction Transaction( LOCTEXT("RemoveGraph", "Remove Graph") );
		GetBlueprintObj()->Modify();

		InGraph->Modify();

		if (InGraphType == EEdGraphSchemaAction_K2Graph::Subgraph)
		{
			// Remove any composite nodes bound to this graph
			TArray<UK2Node_Composite*> AllCompositeNodes;
			FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Composite>(GetBlueprintObj(), AllCompositeNodes);

			const bool bDontRecompile = true;
			for (UK2Node_Composite* CompNode : AllCompositeNodes)
			{
				if (CompNode->BoundGraph == InGraph)
				{
					FBlueprintEditorUtils::RemoveNode(GetBlueprintObj(), CompNode, bDontRecompile);
				}
			}
		}

		FBlueprintEditorUtils::RemoveGraph(GetBlueprintObj(), InGraph, EGraphRemoveFlags::Recompile);
		BlueprintEditorPtr.Pin()->CloseDocumentTab(InGraph);

		for (TObjectIterator<UK2Node_CreateDelegate> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFlags */ EInternalObjectFlags::Garbage); It; ++It)
		{
			if (It->GetGraph() != InGraph)
			{
				if (IsValid(*It) && IsValid(It->GetGraph()))
				{
					It->HandleAnyChange();
				}
			}
		}

		InGraph = NULL;
	}
}

UEdGraph* SCadenceGraphDetailsTabWidget::GetFocusedGraph() const
{
	TSharedPtr<FBlueprintEditor> BlueprintEditorPtrPinned = BlueprintEditorPtr.Pin();
	if( BlueprintEditorPtrPinned.IsValid() )
	{
		return BlueprintEditorPtrPinned->GetFocusedGraph();
	}

	return EdGraph;
}

void SCadenceGraphDetailsTabWidget::OnObjectPropertyChanged(UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent)
{
	if (InObject == CadenceGraph && (InPropertyChangedEvent.ChangeType != EPropertyChangeType::ValueSet && InPropertyChangedEvent.ChangeType != EPropertyChangeType::ArrayClear))
	{
		bNeedsRefresh = true;
	}
}

bool SCadenceGraphDetailsTabWidget::IsEditingMode() const
{
	TSharedPtr<FBlueprintEditor> BlueprintEditorSPtr = BlueprintEditorPtr.Pin();
	return BlueprintEditorSPtr.IsValid() && BlueprintEditorSPtr->InEditingMode();
}

bool SCadenceGraphDetailsTabWidget::IsAnInterfaceEvent(FEdGraphSchemaAction_K2Graph* InAction)
{
	return InAction->GraphType == EEdGraphSchemaAction_K2Graph::Interface && !InAction->EdGraph;
}

void SCadenceGraphDetailsTabWidget::OnDeleteDelegate(FEdGraphSchemaAction_K2Delegate* InDelegateAction)
{
	UEdGraph* GraphToActOn = InDelegateAction->EdGraph;
	UBlueprint* BlueprintObj = GetBlueprintObj();
	if (GraphToActOn && BlueprintObj)
	{
		const FScopedTransaction Transaction( LOCTEXT("RemoveDelegate", "Remove Event Dispatcher") );
		BlueprintObj->Modify();

		BlueprintEditorPtr.Pin()->CloseDocumentTab(GraphToActOn);
		GraphToActOn->Modify();

		FBlueprintEditorUtils::RemoveMemberVariable(BlueprintObj, GraphToActOn->GetFName());
		FBlueprintEditorUtils::RemoveGraph(BlueprintObj, GraphToActOn, EGraphRemoveFlags::Recompile);

		for (TObjectIterator<UK2Node_CreateDelegate> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFlags */ EInternalObjectFlags::Garbage); It; ++It)
		{
			if (IsValid(*It) && IsValid(It->GetGraph()))
			{
				It->HandleAnyChange();
			}
		}
	}
}

namespace UE::Blueprint::Private
{
	// Given a type and value name, display a deletion confirmation warning.
	// Returns true if the user 'cancelled' the action, interpreted as an early exit prior to deletion.
	static bool DisplayInUseWarningAndEarlyExit(const FName& DisplayTypeName, const FName& DisplayValueName)
	{
		const FText DeleteConfirmationPrompt = FText::Format(LOCTEXT("DeleteConfirmationPrompt", "{0} {1} is in use! Do you really want to delete it?")
			, { FText::FromName(DisplayTypeName), FText::FromName(DisplayValueName) }
		);
		const FText DeleteConfirmationTitle = FText::Format(LOCTEXT("DeleteConfirmationTitle", "Delete {0}")
			, { FText::FromName(DisplayTypeName) }
		);
		const FString DeleteConfirmationIniSetting = FString::Format(TEXT("DeleteConfirmation{0}_Warning"), { DisplayTypeName.ToString() });

		// Warn the user that this may result in data loss
		FSuppressableWarningDialog::FSetupInfo Info(DeleteConfirmationPrompt, DeleteConfirmationTitle, DeleteConfirmationIniSetting);
		Info.ConfirmText = LOCTEXT("DeleteConfirmation_Yes", "Yes");
		Info.CancelText = LOCTEXT("DeleteConfirmation_No", "No");

		FSuppressableWarningDialog DeleteFunctionInUse(Info);
		return DeleteFunctionInUse.ShowModal() == FSuppressableWarningDialog::Cancel;
	}
}

void SCadenceGraphDetailsTabWidget::OnDeleteEntry()
{
	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Currently only function graphs are supported for in-use detection and deletion warnings
		if (GraphAction->GraphType == EEdGraphSchemaAction_K2Graph::Function)
		{
			if (FBlueprintEditorUtils::IsFunctionUsed(GetBlueprintObj(), GraphAction->FuncName))
			{
				if (UE::Blueprint::Private::DisplayInUseWarningAndEarlyExit("Function", GraphAction->FuncName))
				{
					return;
				}
			}
		}

		OnDeleteGraph(GraphAction->EdGraph, GraphAction->GraphType);
	}
	else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
	{
		OnDeleteDelegate(DelegateAction);
	}
	else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		if(FBlueprintEditorUtils::IsVariableUsed(GetBlueprintObj(), VarAction->GetVariableName()))
		{
			if (UE::Blueprint::Private::DisplayInUseWarningAndEarlyExit("Variable", VarAction->GetVariableName()))
			{
				return;
			}
		}

		const FScopedTransaction Transaction( LOCTEXT( "RemoveVariable", "Remove Variable" ) );

		GetBlueprintObj()->Modify();
		FBlueprintEditorUtils::RemoveMemberVariable(GetBlueprintObj(), VarAction->GetVariableName());
	}
	else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
	{
		if (FBlueprintEditorUtils::IsVariableUsed(GetBlueprintObj(), LocalVarAction->GetVariableName(), FBlueprintEditorUtils::FindScopeGraph(GetBlueprintObj(), CastChecked<UStruct>(LocalVarAction->GetVariableScope()))))
		{
			if (UE::Blueprint::Private::DisplayInUseWarningAndEarlyExit("Local Variable", LocalVarAction->GetVariableName()))
			{
				return;
			}
		}

		const FScopedTransaction Transaction( LOCTEXT( "RemoveLocalVariable", "Remove Local Variable" ) );

		GetBlueprintObj()->Modify();

		UEdGraph* FunctionGraph = FBlueprintEditorUtils::GetTopLevelGraph(GetFocusedGraph());
		TArray<UK2Node_FunctionEntry*> FunctionEntryNodes;
		FunctionGraph->GetNodesOfClass<UK2Node_FunctionEntry>(FunctionEntryNodes);
		check(FunctionEntryNodes.Num() == 1);
		FunctionEntryNodes[0]->Modify();

		FBlueprintEditorUtils::RemoveLocalVariable(GetBlueprintObj(), CastChecked<UStruct>(LocalVarAction->GetVariableScope()), LocalVarAction->GetVariableName());
	}
	else if (FEdGraphSchemaAction_BlueprintVariableBase* BPVarAction = SelectionAsBlueprintVariable())
	{
		if (BPVarAction->IsVariableUsed())
		{
			if (UE::Blueprint::Private::DisplayInUseWarningAndEarlyExit("Variable", BPVarAction->GetVariableName()))
			{
				return;
			}
		}

		const FScopedTransaction Transaction( LOCTEXT( "RemoveLocalVariable", "Remove Local Variable" ) );

		GetBlueprintObj()->Modify();
		BPVarAction->DeleteVariable();		
	}
	else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
	{
		const FScopedTransaction Transaction(LOCTEXT( "RemoveEventNode", "Remove EventNode"));

		GetBlueprintObj()->Modify();
		FBlueprintEditorUtils::RemoveNode(GetBlueprintObj(), EventAction->NodeTemplate);
	}
	else if (SelectionIsCategory())
	{
		TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
		GraphActionMenu->GetSelectedCategorySubActions(Actions);
		if (Actions.Num())
		{
			FText TransactionTitle;

			switch((CadenceNodeSectionID::Type)Actions[0]->GetSectionID())
			{
			case CadenceNodeSectionID::VARIABLE:
			case CadenceNodeSectionID::LOCAL_VARIABLE:
				{
					TransactionTitle = LOCTEXT( "BulkRemoveVariables", "Bulk Remove Variables" );
					break;
				}
			case CadenceNodeSectionID::DELEGATE:
				{
					TransactionTitle = LOCTEXT( "BulkRemoveDelegates", "Bulk Remove Delegates" );
					break;
				}
			case CadenceNodeSectionID::FUNCTION:
				{
					TransactionTitle = LOCTEXT( "BulkRemoveFunctions", "Bulk Remove Functions" );
					break;
				}
			case CadenceNodeSectionID::MACRO:
				{
					TransactionTitle = LOCTEXT( "BulkRemoveMacros", "Bulk Remove Macros" );
					break;
				}
			default:
				{
					TransactionTitle = LOCTEXT( "BulkRemove", "Bulk Remove Items" );
				}
			}

			FScopedTransaction Transaction( TransactionTitle);

			bool bModified = false;

			GetBlueprintObj()->Modify();
			for (int32 i = 0; i < Actions.Num(); ++i)
			{
				if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Var::StaticGetTypeId())
				{
					FEdGraphSchemaAction_K2Var* Var = (FEdGraphSchemaAction_K2Var*)Actions[i].Get();
					
					FBlueprintEditorUtils::RemoveMemberVariable(GetBlueprintObj(), Var->GetVariableName());
					bModified = true;
				}
				else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2LocalVar::StaticGetTypeId())
				{
					FEdGraphSchemaAction_K2LocalVar* K2LocalVarAction = (FEdGraphSchemaAction_K2LocalVar*)Actions[i].Get();

					FBlueprintEditorUtils::RemoveLocalVariable(GetBlueprintObj(), CastChecked<UStruct>(K2LocalVarAction->GetVariableScope()), K2LocalVarAction->GetVariableName());
					bModified = true;
				}
				else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Graph::StaticGetTypeId())
				{
					FEdGraphSchemaAction_K2Graph* K2GraphAction = (FEdGraphSchemaAction_K2Graph*)Actions[i].Get();
					if(K2GraphAction->EdGraph->bAllowDeletion)
					{
						OnDeleteGraph(K2GraphAction->EdGraph, K2GraphAction->GraphType);
						bModified = true;
					}
				}
				else if (Actions[i]->GetTypeId() == FEdGraphSchemaAction_K2Delegate::StaticGetTypeId())
				{
					OnDeleteDelegate((FEdGraphSchemaAction_K2Delegate*)Actions[i].Get());
					bModified = true;
				}
			}

			if(!bModified)
			{
				Transaction.Cancel();
			}
		}
	}

	Refresh();
	BlueprintEditorPtr.Pin()->GetInspector()->ShowDetailsForObjects(TArray<UObject*>());
}

struct FDeleteEntryHelper
{
	static bool CanDeleteVariable(const UBlueprint* Blueprint, FName VarName)
	{
		check(NULL != Blueprint);

		const FProperty* VariableProperty = FindFProperty<FProperty>(Blueprint->SkeletonGeneratedClass, VarName);
		const UClass* VarSourceClass = VariableProperty->GetOwnerChecked<const UClass>();
		const bool bIsBlueprintVariable = (VarSourceClass == Blueprint->SkeletonGeneratedClass);
		const int32 VarInfoIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VariableProperty->GetFName());
		const bool bHasVarInfo = (VarInfoIndex != INDEX_NONE);

		return bIsBlueprintVariable && bHasVarInfo;
	}
};

bool SCadenceGraphDetailsTabWidget::CanDeleteEntry() const
{
	// Cannot delete entries while not in editing mode
	if(!IsEditingMode())
	{
		return false;
	}

	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		return (GraphAction->EdGraph ? GraphAction->EdGraph->bAllowDeletion : false);
	}
	else if (FEdGraphSchemaAction_K2Delegate* DelegateAction = SelectionAsDelegate())
	{
		return (DelegateAction->EdGraph != nullptr) && (DelegateAction->EdGraph->bAllowDeletion) &&
			FDeleteEntryHelper::CanDeleteVariable(GetBlueprintObj(), DelegateAction->GetDelegateName());
	}
	else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		return FDeleteEntryHelper::CanDeleteVariable(GetBlueprintObj(), VarAction->GetVariableName());
	}
	else if (FEdGraphSchemaAction_K2Event* EventAction = SelectionAsEvent())
	{
		return EventAction->NodeTemplate != nullptr;
	}
	else if (FEdGraphSchemaAction_K2LocalVar* LocalVariable = SelectionAsLocalVar())
	{
		return true;
	}
	else if (FEdGraphSchemaAction_BlueprintVariableBase* BPVariable = SelectionAsBlueprintVariable())
	{
		return true;
	}
	else if (SelectionIsCategory())
	{
		// Can't delete categories if they can't be renamed, that means they are native
		if(GraphActionMenu->CanRequestRenameOnActionNode())
		{
			return true;
		}
	}
	else if (FEdGraphSchemaAction* Action = SelectionAsType<FEdGraphSchemaAction>(GraphActionMenu))
	{
		return Action->CanBeDeleted();
	}

	return false;
}

bool SCadenceGraphDetailsTabWidget::IsDuplicateActionVisible() const
{
	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Functions in interface Blueprints cannot be duplicated
		if(GetBlueprintObj()->BlueprintType != BPTYPE_Interface)
		{
			// Only display it for valid function graphs
			return GraphAction->EdGraph && GraphAction->EdGraph->GetSchema()->CanDuplicateGraph(GraphAction->EdGraph);
		}
	}
	else if (SelectionAsVar() || SelectionAsLocalVar())
	{
		return true;
	}
	return false;
}

bool SCadenceGraphDetailsTabWidget::CanDuplicateAction() const
{
	// Cannot delete entries while not in editing mode
	if (!IsEditingMode())
	{
		return false;
	}

	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Only support function graph duplication
		if(GraphAction->EdGraph)
		{
			return GraphAction->EdGraph->GetSchema()->CanDuplicateGraph(GraphAction->EdGraph);
		}
	}
	else if(FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		// if the property is not an allowable Blueprint variable type, do not allow the variable to be duplicated.
		// Some actions (timelines) exist as variables but cannot be used in a user-defined variable.
		const FObjectPropertyBase* ObjectProperty = CastField<const FObjectPropertyBase>(VarAction->GetProperty());
		if (ObjectProperty &&
			ObjectProperty->PropertyClass &&
			!UEdGraphSchema_K2::IsAllowableBlueprintVariableType(ObjectProperty->PropertyClass))
		{
			return false;
		}
		return true;
	}
	else if(SelectionAsBlueprintVariable())
	{
		return true;
	}
	return false;
}

void SCadenceGraphDetailsTabWidget::OnDuplicateAction()
{
	FName DuplicateActionName = NAME_None;

	if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		// Only StateMachine, function, anim graph and macro duplication is supported
		EGraphType GraphType = GraphAction->EdGraph->GetSchema()->GetGraphType(GraphAction->EdGraph);
		check(GraphType == GT_StateMachine || GraphType == GT_Function || GraphType == GT_Macro || GraphType == GT_Animation);

		if (GraphType == GT_StateMachine)
		{
			// StateMachine is handled using the BlueprintEditor copy / paste functionality
			if (const UAnimationStateMachineGraph* AnimationStateMachineGraph = Cast<UAnimationStateMachineGraph>(GraphAction->EdGraph))
			{
				BlueprintEditorPtr.Pin()->SelectAndDuplicateNode(AnimationStateMachineGraph->OwnerAnimGraphNode.Get());
			}
		}
		else
		{
			const FScopedTransaction Transaction(LOCTEXT("DuplicateGraph", "Duplicate Graph"));
			GetBlueprintObj()->Modify();

			UEdGraph* DuplicatedGraph = GraphAction->EdGraph->GetSchema()->DuplicateGraph(GraphAction->EdGraph);
			check(DuplicatedGraph);

			DuplicatedGraph->Modify();

			// Generate new Guids and component templates for all relevant nodes in the graph
			// *NOTE* this cannot occur during PostDuplicate, node Guids and component templates need to remain static during duplication for Blueprint compilation
			for (UEdGraphNode* EdGraphNode : DuplicatedGraph->Nodes)
			{
				if (EdGraphNode)
				{
					EdGraphNode->CreateNewGuid();

					if (UK2Node_AddComponent* AddComponentNode = Cast<UK2Node_AddComponent>(EdGraphNode))
					{
						AddComponentNode->MakeNewComponentTemplate();
					}
				}
			}

			if (GraphType == GT_Function || GraphType == GT_Animation)
			{
				GetBlueprintObj()->FunctionGraphs.Add(DuplicatedGraph);
			}
			else if (GraphType == GT_Macro)
			{
				GetBlueprintObj()->MacroGraphs.Add(DuplicatedGraph);
			}
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprintObj());

			BlueprintEditorPtr.Pin()->OpenDocument(DuplicatedGraph, FDocumentTracker::ForceOpenNewDocument);
			DuplicateActionName = DuplicatedGraph->GetFName();
		}
	}
	else if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		const FScopedTransaction Transaction( LOCTEXT( "DuplicateVariable", "Duplicate Variable" ) );
		GetBlueprintObj()->Modify();

		DuplicateActionName = FBlueprintEditorUtils::DuplicateVariable(GetBlueprintObj(), nullptr, VarAction->GetVariableName());
		if(DuplicateActionName == NAME_None)
		{
			// the variable was probably inherited from a C++ class

			FEdGraphPinType VarPinType;
			GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(VarAction->GetProperty(), VarPinType);
			FBlueprintEditorUtils::AddMemberVariable(GetBlueprintObj(), FBlueprintEditorUtils::FindUniqueKismetName(CadenceGraph, VarAction->GetVariableName().ToString()), VarPinType);
		}
	}
	else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
	{
		const FScopedTransaction Transaction( LOCTEXT( "Duplicate Local Variable", "Duplicate Local Variable" ) );
		GetBlueprintObj()->Modify();

		DuplicateActionName = FBlueprintEditorUtils::DuplicateVariable(GetBlueprintObj(), Cast<UStruct>(LocalVarAction->GetVariableScope()), LocalVarAction->GetVariableName());
	}

	// Select and rename the duplicated action
	if(DuplicateActionName != NAME_None)
	{
		SelectItemByName(DuplicateActionName);
		Refresh();
		OnRequestRenameOnActionNode();
	}
}

void SCadenceGraphDetailsTabWidget::OnCopy()
{
	FString OutputString;

	if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		UBlueprint* SourceBlueprint;
		int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndexAndBlueprint(CadenceGraph, VarAction->GetVariableName(), SourceBlueprint);
		if (VarIndex != INDEX_NONE)
		{
			// make a copy of the Variable description so we can set the default value
			FBPVariableDescription Description = SourceBlueprint->NewVariables[VarIndex];

			//Grab property of blueprint's current CDO
			UClass* GeneratedClass = SourceBlueprint->GeneratedClass;
			UObject* GeneratedCDO = GeneratedClass->GetDefaultObject();
			FProperty* TargetProperty = FindFProperty<FProperty>(GeneratedClass, Description.VarName);

			if (TargetProperty)
			{
				// Grab the address of where the property is actually stored (UObject* base, plus the offset defined in the property)
				void* OldPropertyAddr = TargetProperty->ContainerPtrToValuePtr<void>(GeneratedCDO);
				if (OldPropertyAddr)
				{
					TargetProperty->ExportTextItem_Direct(Description.DefaultValue, OldPropertyAddr, OldPropertyAddr, nullptr, PPF_SerializedAsImportText);
				}
			}

			FBPVariableDescription::StaticStruct()->ExportText(OutputString, &Description, &Description, nullptr, 0, nullptr, false);
			OutputString = VAR_PREFIX + OutputString;
		}
	}
	else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
	{
		FBPVariableDescription* Description = FBlueprintEditorUtils::FindLocalVariable(CadenceGraph, CastChecked<UStruct>(LocalVarAction->GetVariableScope()), LocalVarAction->GetVariableName());

		if (Description)
		{
			FBPVariableDescription::StaticStruct()->ExportText(OutputString, Description, Description, nullptr, 0, nullptr, false);
			OutputString = VAR_PREFIX + OutputString;
		}
	}
	else if (FEdGraphSchemaAction_BlueprintVariableBase* BPVariable = SelectionAsBlueprintVariable())
	{
		if (const UEdGraph* FocusedGraph = Cast<UEdGraph>(BPVariable->GetVariableScope()))
		{
			if (const UEdGraphSchema* Schema = FocusedGraph->GetSchema())
			{
				TArray<FBPVariableDescription> LocalVariables;
				Schema->GetLocalVariables(FocusedGraph, LocalVariables);
				for (const FBPVariableDescription& VariableDescription : LocalVariables)
				{
					if (VariableDescription.VarName == BPVariable->GetVariableName())
					{
						FBPVariableDescription::StaticStruct()->ExportText(OutputString, &VariableDescription, &VariableDescription, nullptr, 0, nullptr, false);
						OutputString = VAR_PREFIX + OutputString;
						break;
					}
				}
			}
		}
	}
	else if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		if (!CadenceGraph->ExportGraphToText(GraphAction->EdGraph, OutputString))
		{
			FBPGraphClipboardData FuncData(GraphAction->EdGraph);
			FBPGraphClipboardData::StaticStruct()->ExportText(OutputString, &FuncData, &FuncData, nullptr, 0, nullptr, false);
			OutputString = GRAPH_PREFIX + OutputString;
		}
	}

	if (!OutputString.IsEmpty())
	{
		FPlatformApplicationMisc::ClipboardCopy(OutputString.GetCharArray().GetData());
	}
}

bool SCadenceGraphDetailsTabWidget::CanCopy() const
{
	if (FEdGraphSchemaAction_K2Var* VarAction = SelectionAsVar())
	{
		return FBlueprintEditorUtils::FindNewVariableIndex(CadenceGraph, VarAction->GetVariableName()) != INDEX_NONE;
	}
	else if (FEdGraphSchemaAction_K2LocalVar* LocalVarAction = SelectionAsLocalVar())
	{
		return FBlueprintEditorUtils::FindLocalVariable(CadenceGraph, Cast<UStruct>(LocalVarAction->GetVariableScope()), LocalVarAction->GetVariableName()) != nullptr;
	}
	else if (FEdGraphSchemaAction_BlueprintVariableBase* BPVariable = SelectionAsBlueprintVariable())
	{
		if (const UEdGraph* FocusedGraph = Cast<UEdGraph>(BPVariable->GetVariableScope()))
		{
			if (const UEdGraphSchema* Schema = FocusedGraph->GetSchema())
			{
				TArray<FBPVariableDescription> LocalVariables;
				Schema->GetLocalVariables(FocusedGraph, LocalVariables);

				for (const FBPVariableDescription& VariableDescription : LocalVariables)
				{
					if (VariableDescription.VarName == BPVariable->GetVariableName())
					{
						return true;
					}
				}
			}
		}
		return false;		
	}
	else if (FEdGraphSchemaAction_K2Graph* GraphAction = SelectionAsGraph())
	{
		if (GraphAction->GraphType == EEdGraphSchemaAction_K2Graph::Function ||
			GraphAction->GraphType == EEdGraphSchemaAction_K2Graph::Macro)
		{
			return true;
		}
	}

	return false;
}

void SCadenceGraphDetailsTabWidget::OnCut()
{
	OnCopy();
	OnDeleteEntry();
}

bool SCadenceGraphDetailsTabWidget::CanCut() const
{
	return CanCopy() && CanDeleteEntry();
}

void SCadenceGraphDetailsTabWidget::OnPasteGeneric()
{
	// prioritize pasting as a member variable if possible
	if (CanPasteVariable())
	{
		OnPasteVariable();
	}
}

bool SCadenceGraphDetailsTabWidget::CanPasteGeneric()
{
	return CanPasteVariable();
}

void SCadenceGraphDetailsTabWidget::OnPasteVariable()
{
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);
	if (!ensure(ClipboardText.StartsWith(VAR_PREFIX, ESearchCase::CaseSensitive)))
	{
		return;
	}

	FBPVariableDescription Description;
	FStringOutputDevice Errors;
	const TCHAR* Import = ClipboardText.GetCharArray().GetData() + FCString::Strlen(VAR_PREFIX);
	FBPVariableDescription::StaticStruct()->ImportText(Import, &Description, nullptr, PPF_None, &Errors, FBPVariableDescription::StaticStruct()->GetName());
	if (Errors.IsEmpty())
	{
		FBPVariableDescription NewVar = FBlueprintEditorUtils::DuplicateVariableDescription(CadenceGraph, Description);
		if (NewVar.VarGuid.IsValid())
		{
			FScopedTransaction Transaction(FText::Format(LOCTEXT("PasteVariable", "Paste Variable: {0}"), FText::FromName(NewVar.VarName)));
			CadenceGraph->Modify();

			NewVar.Category = GetPasteCategory();

			CadenceGraph->NewVariables.Add(NewVar);

			// Potentially adjust variable names for any child blueprints
			FBlueprintEditorUtils::ValidateBlueprintChildVariables(CadenceGraph, NewVar.VarName);
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(CadenceGraph);

			SelectItemByName(NewVar.VarName);
		}
	}
}

bool SCadenceGraphDetailsTabWidget::CanPasteVariable() const
{
	TSharedPtr<FBlueprintEditor> PinnedEditor = BlueprintEditorPtr.Pin();
	if (PinnedEditor.IsValid() && !PinnedEditor->NewDocument_IsVisibleForType(FBlueprintEditor::CGT_NewVariable))
	{
		return false;
	}

	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);
	if (ClipboardText.StartsWith(VAR_PREFIX, ESearchCase::CaseSensitive))
	{
		FBPVariableDescription Description;
		FStringOutputDevice Errors;
		const TCHAR* Import = ClipboardText.GetCharArray().GetData() + FCString::Strlen(VAR_PREFIX);
		FBPVariableDescription::StaticStruct()->ImportText(Import, &Description, nullptr, 0, &Errors, FBPVariableDescription::StaticStruct()->GetName());

		return Errors.IsEmpty();
	}

	return false;
}

FText SCadenceGraphDetailsTabWidget::GetPasteCategory() const
{
	if (SelectionIsCategory() && GraphActionMenu.IsValid())
	{
		FString CategoryName = GraphActionMenu->GetSelectedCategoryName();
		if (!CategoryName.IsEmpty())
		{
			return FText::FromString(GraphActionMenu->GetSelectedCategoryName());
		}
	}
	
	return UEdGraphSchema_K2::VR_DefaultCategory;
}

void SCadenceGraphDetailsTabWidget::OnResetItemFilter()
{
	FilterBox->SetText(FText::GetEmpty());
}

void SCadenceGraphDetailsTabWidget::EnsureLastPinTypeValid()
{
	LastPinType.bIsWeakPointer = false;
	LastFunctionPinType.bIsWeakPointer = false;

	const bool bLastPinTypeValid = (UEdGraphSchema_K2::PC_Struct != LastPinType.PinCategory) || LastPinType.PinSubCategoryObject.IsValid();
	const bool bLastFunctionPinTypeValid = (UEdGraphSchema_K2::PC_Struct != LastFunctionPinType.PinCategory) || LastFunctionPinType.PinSubCategoryObject.IsValid();
	const bool bConstType = LastPinType.bIsConst || LastFunctionPinType.bIsConst;
	if (!bLastPinTypeValid || !bLastFunctionPinTypeValid || bConstType)
	{
		ResetLastPinType();
	}
}

void SCadenceGraphDetailsTabWidget::ResetLastPinType()
{
	LastPinType.ResetToDefaults();
	LastPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	LastFunctionPinType = LastPinType;
}

void SCadenceGraphDetailsTabWidget::UpdateNodeCreation()
{
	if( BlueprintEditorPtr.IsValid() )
	{
		BlueprintEditorPtr.Pin()->UpdateNodeCreationStats( ENodeCreateAction::MyBlueprintDragPlacement );
	}
}

void SCadenceGraphDetailsTabWidget::OnFilterTextChanged( const FText& InFilterText )
{
	GraphActionMenu->GenerateFilteredItems(false);
}

FText SCadenceGraphDetailsTabWidget::GetFilterText() const
{
	return FilterBox->GetText();
}

void SCadenceGraphDetailsTabWidget::OnRequestRenameOnActionNode()
{
	// Attempt to rename in both menus, only one of them will have anything selected
	GraphActionMenu->OnRequestRenameOnActionNode();
}

bool SCadenceGraphDetailsTabWidget::CanRequestRenameOnActionNode() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	// If there is anything selected in the GraphActionMenu, check the item for if it can be renamed.
	if (SelectedActions.Num() || SelectionIsCategory())
	{
		return GraphActionMenu->CanRequestRenameOnActionNode();
	}
	return false;
}

void SCadenceGraphDetailsTabWidget::SelectItemByName(const FName& ItemName, ESelectInfo::Type SelectInfo, int32 SectionId/* = INDEX_NONE*/, bool bIsCategory/* = false*/)
{
	// Check if the graph action menu is being told to clear
	if(ItemName == NAME_None)
	{
		ClearGraphActionMenuSelection();
	}
	else
	{
		// Attempt to select the item in the main graph action menu
		const bool bSucceededAtSelecting = GraphActionMenu->SelectItemByName(ItemName, SelectInfo, SectionId, bIsCategory);
		if (!bSucceededAtSelecting)
		{
			// We failed to select the item, maybe because it was filtered out?
			// Reset the item filter and try again (we don't do this first because someone went to the effort of typing
			// a filter and probably wants to keep it unless it is getting in the way, as it just has)
			OnResetItemFilter();
			GraphActionMenu->SelectItemByName(ItemName, SelectInfo, SectionId, bIsCategory);
		}
	}
}

void SCadenceGraphDetailsTabWidget::ClearGraphActionMenuSelection()
{
	GraphActionMenu->SelectItemByName(NAME_None);
}

void SCadenceGraphDetailsTabWidget::ExpandCategory(const FText& CategoryName)
{
	GraphActionMenu->ExpandCategory(CategoryName);
}

bool SCadenceGraphDetailsTabWidget::MoveCategoryBeforeCategory(const FText& InCategoryToMove, const FText& InTargetCategory)
{
	bool bResult = false;

	FString CategoryToMoveString = InCategoryToMove.ToString();
	FString TargetCategoryString = InTargetCategory.ToString();
	if (UBlueprint* BlueprintObj = BlueprintEditorPtr.Pin()->GetBlueprintObj())
	{
		FScopedTransaction Transaction(LOCTEXT("ReorderCategories", "Reorder Categories"));
		BlueprintObj->Modify();

		// Find root categories
		int32 RootCategoryDelim = CategoryToMoveString.Find(TEXT("|"), ESearchCase::CaseSensitive);
		FName CategoryToMove = RootCategoryDelim == INDEX_NONE ? *CategoryToMoveString : *CategoryToMoveString.Left(RootCategoryDelim);
		RootCategoryDelim = TargetCategoryString.Find(TEXT("|"), ESearchCase::CaseSensitive);
		FName TargetCategory = RootCategoryDelim == INDEX_NONE ? *TargetCategoryString : *TargetCategoryString.Left(RootCategoryDelim);

		TArray<FName>& CategorySort = BlueprintObj->CategorySorting;

		// Remove existing sort index
		const int32 RemovalIndex = CategorySort.Find(CategoryToMove);
		if (RemovalIndex != INDEX_NONE)
		{
			CategorySort.RemoveAt(RemovalIndex);
		}

		// Update the Category sort order and refresh ( if the target category has an entry )
		const int32 InsertIndex = CategorySort.Find(TargetCategory);
		if (InsertIndex != INDEX_NONE)
		{
			CategorySort.Insert(CategoryToMove, InsertIndex);
			Refresh();
			bResult = true;
		}
	}

	return bResult;
}