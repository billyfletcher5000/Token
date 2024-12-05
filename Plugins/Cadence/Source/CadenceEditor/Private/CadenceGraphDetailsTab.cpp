// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphDetailsTab.h"

#include "CadenceGraphApplication.h"
#include "CadenceGraphSchemaActions.h"
#include "CadencePalette.h"
#include "EditorCategoryUtils.h"
#include "GraphActionNode.h"
#include "GraphEditorDragDropAction.h"
#include "SGraphActionMenu.h"
#include "SPositiveActionButton.h"
#include "UnrealExporter.h"
#include "Android/AndroidPlatformApplicationMisc.h"
#include "Dialogs/Dialogs.h"
#include "Exporters/Exporter.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/CadenceGraph.h"
#include "Widgets/Input/SSearchBox.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

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

TSharedPtr<SWidget> SCadenceGraphDetailsTabWidget::OnContextMenuOpening()
{
	if( !ApplicationPtr.IsValid() )
	{
		return TSharedPtr<SWidget>();
	}

	const bool bShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder( bShouldCloseWindowAfterMenuSelection, CommandList);
	
	// Check if the selected action is valid for a context menu
	if (SelectionHasContextMenu())
	{
		FCadenceVariableAction* Var = SelectionAsVar();
		FCadenceGraphAction* Graph = SelectionAsGraph();
		
		MenuBuilder.BeginSection("BasicOperations");
		{
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().OpenExternalGraph);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().FocusNode);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"), LOCTEXT("Rename_Tooltip", "Renames this function or variable from blueprint.") );
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().GotoNativeVarDefinition);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
			MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().DeleteEntry);
		}
		MenuBuilder.EndSection();
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

	MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().AddNewVariable);
	MenuBuilder.AddMenuEntry(FCadenceGraphDetailsCommands::Get().PasteVariable);
	
	MenuBuilder.EndSection();
}

bool SCadenceGraphDetailsTabWidget::CanOpenExternalGraph() const 
{
	const FCadenceGraphAction* GraphAction = SelectionAsGraph();
	const bool bGraph = GraphAction && GraphAction->EdGraph;
	return bGraph;
}

void SCadenceGraphDetailsTabWidget::OpenGraph(FDocumentTracker::EOpenDocumentCause InCause, bool bOpenExternalGraphInNewEditor)
{
	UCadenceAsset* GraphToOpen = nullptr;

	if (FCadenceGraphAction* GraphAction = SelectionAsGraph())
	{
		GraphToOpen = GraphAction->CadenceAsset;
	}
	
	if (GraphToOpen)
	{
		if(bOpenExternalGraphInNewEditor)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(GraphToOpen);
		}
		else
		{			
			TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(GraphToOpen);
			ApplicationPtr.Pin()->GetDocumentManager()->OpenDocument(Payload, InCause);
		}
	}
}


void SCadenceGraphDetailsTabWidget::OnOpenExternalGraph()
{
	OpenGraph(FDocumentTracker::OpenNewDocument, true);
}

bool SCadenceGraphDetailsTabWidget::CanFocusOnNode() const
{
	return false;
}

void SCadenceGraphDetailsTabWidget::OnFocusNode()
{
}

void SCadenceGraphDetailsTabWidget::OnDeleteGraph(UEdGraph* InGraph, ECadenceGraphAction::Type InGraphType)
{
	if (InGraph && InGraph->bAllowDeletion)
	{
		// TODO
	}
}

UEdGraph* SCadenceGraphDetailsTabWidget::GetFocusedGraph() const
{
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
	TSharedPtr<FCadenceGraphApplication> App = ApplicationPtr.Pin();
	return App.IsValid() && App->InEditingMode();
}

namespace Cadence::Private
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

	FName FindUniqueName(const UCadenceGraph* InGraph, const FName& InBaseName)
	{
		if(InBaseName.ToString.IsEmpty())
			return NAME_None;
		
		int32 Count = 0;
		FString BaseName = InBaseName.ToString();
		FString NewName;

		TArray<FName> ExistingNames;
		
		for(const FCadenceNamedVariable& NamedVar : InGraph->UserVariables.Variables)
		{
			ExistingNames.Add(NamedVar.Variable->GetUserVariableName());			
		}
		
		while(Count < 1000)
		{
			// Calculate the number of digits in the number, adding 2 (1 extra to correctly count digits, another to account for the '_' that will be added to the name
			int32 CountLength = Count > 0? (int32)log((double)Count) + 2 : 2;

			NewName = FString::Printf(TEXT("%s_%d"), *BaseName, Count);

			if(!ExistingNames.Contains(NewName))
				return FName(NewName);
			
			Count++;
		}

		return NAME_None;
	}

	FString ExportObjectToText(UObject* ObjectToExport)
	{
		// Clear the mark state for saving.
		UnMarkAllObjects(EObjectMark(OBJECTMARK_TagExp | OBJECTMARK_TagImp));

		FStringOutputDevice Archive;
		const FExportObjectInnerContext Context;

		UObject* ThisOuter = ObjectToExport->GetOuter();
		UExporter::ExportToOutputDevice(&Context, ObjectToExport, NULL, Archive, TEXT("copy"), 0, PPF_ExportsNotFullyQualified|PPF_Copy|PPF_Delimited, false, ThisOuter);

		return *Archive;
	}
}

void SCadenceGraphDetailsTabWidget::OnDeleteEntry()
{
	if (FCadenceGraphAction* GraphAction = SelectionAsGraph())
	{
		OnDeleteGraph(GraphAction->EdGraph, GraphAction->GraphType);
	}
	else if (FCadenceVariableAction* VarAction = SelectionAsVar())
	{
		const FScopedTransaction Transaction( LOCTEXT( "RemoveVariable", "Remove Variable" ) );

		UCadenceVariable* Variable = VarAction->GetVariable();
		UCadenceGraph* MainGraph = GetMainGraph();
		MainGraph->Modify();
		int32 Index = MainGraph->UserVariables.Variables.IndexOfByPredicate(
			[Variable](const FCadenceNamedVariable& InNamedVar)
			{
				return InNamedVar.Variable == Variable;
			});

		MainGraph->UserVariables.Variables.RemoveAt(Index);
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
				{
					TransactionTitle = LOCTEXT( "BulkRemoveVariables", "Bulk Remove Variables" );
					break;
				}
			default:
				{
					TransactionTitle = LOCTEXT( "BulkRemove", "Bulk Remove Items" );
				}
			}

			FScopedTransaction Transaction( TransactionTitle);

			bool bModified = false;

			UCadenceGraph* MainGraph = GetMainGraph();
			MainGraph->Modify();
			
			for (int32 i = 0; i < Actions.Num(); ++i)
			{
				if (Actions[i]->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
				{
					FCadenceVariableAction* CurrentVarAction = (FCadenceVariableAction*)Actions[i].Get();
					UCadenceVariable* Variable = CurrentVarAction->GetVariable();
					int32 Index = MainGraph->UserVariables.Variables.IndexOfByPredicate(
						[Variable](const FCadenceNamedVariable& InNamedVar)
						{
							return InNamedVar.Variable == Variable;
						});

					MainGraph->UserVariables.Variables.RemoveAt(Index);
				}
			}

			if(!bModified)
			{
				Transaction.Cancel();
			}
		}
	}

	Refresh();
	
	ApplicationPtr.Pin()->GetSelectedDetailsView()->SetObject(nullptr);	
}


bool SCadenceGraphDetailsTabWidget::CanDeleteEntry() const
{
	// Cannot delete entries while not in editing mode
	if(!IsEditingMode())
	{
		return false;
	}

	if (FCadenceVariableAction* VarAction = SelectionAsVar())
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
	if (SelectionAsVar())
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

	if (FCadenceGraphAction* GraphAction = SelectionAsGraph())
	{
		// Only support function graph duplication
		if(GraphAction->EdGraph)
		{
			return GraphAction->EdGraph->GetSchema()->CanDuplicateGraph(GraphAction->EdGraph);
		}
	}
	else if(FCadenceVariableAction* VarAction = SelectionAsVar())
	{
		return true;
	}
	
	return false;
}

void SCadenceGraphDetailsTabWidget::OnDuplicateAction()
{
	FName DuplicateActionName = NAME_None;

	if (FCadenceVariableAction* VarAction = SelectionAsVar())
	{
		const FScopedTransaction Transaction( LOCTEXT( "DuplicateVariable", "Duplicate Variable" ) );
		UCadenceVariable* Variable = VarAction->GetVariable();
		UCadenceGraph* MainGraph = GetMainGraph();
		MainGraph->Modify();

		DuplicateActionName = Cadence::Private::FindUniqueName(MainGraph, Variable->GetUserVariableName());
		if(DuplicateActionName != NAME_None)
		{
			UCadenceVariable* NewVar = NewObject<UCadenceVariable>(MainGraph, Variable->GetClass());
			NewVar->SetUserVariableName(DuplicateActionName);
			MainGraph->UserVariables.Variables.Add(FCadenceNamedVariable(NewVar));
		}
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

	if (FCadenceVariableAction* VarAction = SelectionAsVar())
	{
		UCadenceVariable* Variable = VarAction->GetVariable();
		OutputString = Cadence::Private::ExportObjectToText(Variable);
	}

	if (!OutputString.IsEmpty())
	{
		FPlatformApplicationMisc::ClipboardCopy(OutputString.GetCharArray().GetData());
	}
}

bool SCadenceGraphDetailsTabWidget::CanCopy() const
{
	if (FCadenceVariableAction* VarAction = SelectionAsVar())
	{
		return true;
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
	// TODO
}

bool SCadenceGraphDetailsTabWidget::CanPasteVariable() const
{
	// TODO
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
	if (UCadenceGraph* MainGraph = GetMainGraph())
	{
		FScopedTransaction Transaction(LOCTEXT("ReorderCategories", "Reorder Categories"));
		MainGraph->Modify();

		// Find root categories
		int32 RootCategoryDelim = CategoryToMoveString.Find(TEXT("|"), ESearchCase::CaseSensitive);
		FName CategoryToMove = RootCategoryDelim == INDEX_NONE ? *CategoryToMoveString : *CategoryToMoveString.Left(RootCategoryDelim);
		RootCategoryDelim = TargetCategoryString.Find(TEXT("|"), ESearchCase::CaseSensitive);
		FName TargetCategory = RootCategoryDelim == INDEX_NONE ? *TargetCategoryString : *TargetCategoryString.Left(RootCategoryDelim);

		TArray<FName>& CategorySort = MainGraph->CategorySorting;

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