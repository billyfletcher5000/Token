// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphApplication.h"

#include "CadenceAsset.h"
#include "CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphSchema.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "IDetailsView.h"


const FName FCadenceGraphApplication::ToolkitFName = FName(TEXT("CadenceGraphApplication"));
const FText FCadenceGraphApplication::BaseToolkitName = FText::FromString(TEXT("CadenceGraphApplication"));
const FString FCadenceGraphApplication::WorldCentricTabPrefix = TEXT("CadenceGraphApplication");
const FLinearColor FCadenceGraphApplication::WorldCentricTabColorScale = FLinearColor(0.549f, 0.0f, 0.784f, 0.5f);
const FString FCadenceGraphApplication::DocumentationLink = TEXT("https://github.com/billyfletcher5000");

void FCadenceGraphApplication::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FCadenceGraphApplication::InitEditor(const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost> InToolkitHost, UObject* InObject)
{
	WorkingAsset = Cast<UCadenceAsset>(InObject);
	ensure(WorkingAsset);

	if (WorkingAsset->GetGraph() == nullptr)
	{
		WorkingAsset->CreateGraph();
	}

	PreSaveDelegateHandle = WorkingAsset->OnPreSaveDelegate.AddRaw(this, &FCadenceGraphApplication::OnWorkingAssetPreSave);

	if(!WorkingGraphEditor)
	{
		UEdGraph* CreatedGraph = FBlueprintEditorUtils::CreateNewGraph(WorkingAsset, NAME_None, UCadenceGraphEditor::StaticClass(), UCadenceGraphSchema::StaticClass());	
		WorkingGraphEditor = Cast<UCadenceGraphEditor>(CreatedGraph);
	} 
	
	ensure(WorkingGraphEditor);
	WorkingGraphEditor->SetRuntimeGraph(WorkingAsset->GetGraph());
	
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(InObject);

	InitAssetEditor(InMode, InToolkitHost, TEXT("Cadence"), FTabManager::FLayout::NullLayout, true, true, ObjectsToEdit);

	AddApplicationMode(FCadenceGraphApplicationMode::ModeName, MakeShareable(new FCadenceGraphApplicationMode(SharedThis(this))));
	SetCurrentMode(FCadenceGraphApplicationMode::ModeName);

	ReconstructEditorGraph();
}

void FCadenceGraphApplication::OnClose()
{
	FWorkflowCentricApplication::OnClose();
	WorkingAsset->OnPreSaveDelegate.Remove(PreSaveDelegateHandle);
}

TSharedPtr<FUICommandList> FCadenceGraphApplication::GetCommandList()
{
	if(!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShareable( new FUICommandList );
		{
			GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
				FExecuteAction::CreateSP( this, &FCadenceGraphApplication::DeleteSelectedNodes ),
				FCanExecuteAction::CreateSP( this, &FCadenceGraphApplication::CanDeleteSelectedNodes )
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
				FExecuteAction::CreateSP( this, &FCadenceGraphApplication::CutSelectedNodes ),
				FCanExecuteAction::CreateSP( this, &FCadenceGraphApplication::HasValidSelection )
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
				FExecuteAction::CreateSP( this, &FCadenceGraphApplication::CopySelectedNodes ),
				FCanExecuteAction::CreateSP( this, &FCadenceGraphApplication::HasValidSelection )
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
				FExecuteAction::CreateSP( this, &FCadenceGraphApplication::DuplicateSelectedNodes ),
				FCanExecuteAction::CreateSP( this, &FCadenceGraphApplication::HasValidSelection )
				);

			GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
				FExecuteAction::CreateSP( this, &FCadenceGraphApplication::PasteClipboardNodes ),
				FCanExecuteAction::CreateSP( this, &FCadenceGraphApplication::HasValidNodesInClipboard )
				);
		}
	}

	return GraphEditorCommands;
}

void FCadenceGraphApplication::SetSelectedDetailsView(const TSharedPtr<IDetailsView>& InDetailsView)
{
	SelectedDetailsView = InDetailsView;
	InDetailsView->OnFinishedChangingProperties().AddSP(this, &FCadenceGraphApplication::OnDetailsPropertyChangesFinished);
}

void FCadenceGraphApplication::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void FCadenceGraphApplication::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}

void FCadenceGraphApplication::OnGraphSelectionChanged(const FGraphPanelSelectionSet& InSelectionSet)
{
	TArray<UObject*> RuntimeNodes;
	for(UObject* Object : InSelectionSet)
	{
		if(UCadenceGraphEditorNode* GraphEditorNode = Cast<UCadenceGraphEditorNode>(Object))
		{
			RuntimeNodes.Add(GraphEditorNode->GetRuntimeGraphNode());
		}
	}
	
	SelectedDetailsView.Pin()->SetObjects(RuntimeNodes);
}

void FCadenceGraphApplication::OnDetailsPropertyChangesFinished(const FPropertyChangedEvent& InPropertyChangedEvent)
{	
	TSharedPtr<SGraphEditor> GraphEditor = SlateGraphEditor.Pin();
	GraphEditor->NotifyGraphChanged();
}

void FCadenceGraphApplication::ReconstructEditorGraph()
{
	UCadenceGraph* RuntimeGraph = WorkingAsset->GetGraph();
	ensure(RuntimeGraph);

	const TArray<TObjectPtr<UCadenceGraphNode>>& RuntimeNodes = RuntimeGraph->GetNodes();

	// Create nodes first
	for(UCadenceGraphNode* RuntimeNode : RuntimeNodes)
	{
		FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*WorkingGraphEditor);
        UCadenceGraphEditorNode* Node = NodeCreator.CreateNode(false);
        Node->Construct(RuntimeNode);
        NodeCreator.Finalize();
        
        WorkingGraphEditor->AddNode(Node, true, true);
	}

	// Connect nodes after
	for(UEdGraphNode* EdGraphNode : WorkingGraphEditor->Nodes)
	{
		UCadenceGraphEditorNode* CadenceEdNode = Cast<UCadenceGraphEditorNode>(EdGraphNode);
		CadenceEdNode->ReconstructConnections();
	}
}

void FCadenceGraphApplication::OnWorkingAssetPreSave()
{
	for(UEdGraphNode* EdGraphNode : WorkingGraphEditor->Nodes)
	{
		UCadenceGraphEditorNode* CadenceEditorNode = Cast<UCadenceGraphEditorNode>(EdGraphNode);
		CadenceEditorNode->UpdateRuntimePosition();
	}
}

void FCadenceGraphApplication::DeleteSelectedNodes() const
{
	TSharedPtr<SGraphEditor> SlateGraph = SlateGraphEditor.Pin();
	const FGraphPanelSelectionSet& SelectedNodes = SlateGraph->GetSelectedNodes();
	
	for (FGraphPanelSelectionSet::TConstIterator NodeIt( SelectedNodes ); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				if(UEdGraph* Graph = Node->GetGraph(); Graph != nullptr)
				{
					Graph->Modify();
				}

				Node->DestroyNode();
			}
		}
	}
}

bool FCadenceGraphApplication::CanDeleteSelectedNodes() const
{
	TSharedPtr<SGraphEditor> SlateGraph = SlateGraphEditor.Pin();
	const FGraphPanelSelectionSet& SelectedNodes = SlateGraph->GetSelectedNodes();

	if (SelectedNodes.Num() == 0)
		return false;
	
	for (FGraphPanelSelectionSet::TConstIterator NodeIt( SelectedNodes ); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (!Node->CanUserDeleteNode())
				return false;
		}
		
	}
	
	return true;
}

void FCadenceGraphApplication::CutSelectedNodes()
{
}

void FCadenceGraphApplication::CopySelectedNodes()
{
}

void FCadenceGraphApplication::DuplicateSelectedNodes()
{
}

bool FCadenceGraphApplication::HasValidSelection() const
{
	TSharedPtr<SGraphEditor> SlateGraph = SlateGraphEditor.Pin();
	const FGraphPanelSelectionSet& SelectedNodes = SlateGraph->GetSelectedNodes();

	if (SelectedNodes.Num() == 0)
		return false;

	return true;
}

void FCadenceGraphApplication::PasteClipboardNodes()
{
}

bool FCadenceGraphApplication::HasValidNodesInClipboard() const
{
	return false;
}

const FName FCadenceGraphApplicationMode::ModeName = FName(TEXT("CadenceGraphApplicationMode"));
const FName FCadenceGraphApplicationMode::LayoutName = FName(TEXT("CadenceGraphApplicationMode_Layout_v1"));

FCadenceGraphApplicationMode::FCadenceGraphApplicationMode(TSharedPtr<FCadenceGraphApplication> InApplication)
: FApplicationMode(ModeName)
{
	Application = InApplication;
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphPrimaryTabFactory(InApplication)));
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphPropertiesTabFactory(InApplication)));

	TabLayout = FTabManager::NewLayout(LayoutName)
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.75f)
				->AddTab(
				FCadenceGraphPrimaryTabFactory::Identifier, ETabState::OpenedTab
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab(
				FCadenceGraphPropertiesTabFactory::Identifier, ETabState::OpenedTab
				)
			)
		)
	);
}

void FCadenceGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FCadenceGraphApplication> SharedApp = Application.Pin();
	SharedApp->PushTabFactories(Tabs);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FCadenceGraphApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FCadenceGraphApplicationMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

const FName FCadenceGraphPrimaryTabFactory::Identifier = FName(TEXT("CadenceGraphPrimaryTab"));

FCadenceGraphPrimaryTabFactory::FCadenceGraphPrimaryTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication)
: FWorkflowTabFactory(Identifier, InApplication)
{
	Application = InApplication;

	TabLabel = FText::FromString(TEXT("Graph"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the main graph view of a Cadence Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the graph view."));
}

TSharedRef<SWidget> FCadenceGraphPrimaryTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FCadenceGraphApplication> App = Application.Pin();

	SGraphEditor::FGraphEditorEvents GraphEditorEvents;
	GraphEditorEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(App.Get(), &FCadenceGraphApplication::OnGraphSelectionChanged);
	
	TSharedRef<SGraphEditor> GraphEditor = 
					SNew(SGraphEditor)
						.IsEditable(true)
						.AdditionalCommands(App->GetCommandList())
						.GraphEvents(GraphEditorEvents)
						.GraphToEdit(App->GetWorkingGraphEditor());
	
	App->SetSlateGraphEditor(GraphEditor);
	
	TSharedRef<SWidget> Widget =
		SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					GraphEditor
				];

	return Widget;
}

FText FCadenceGraphPrimaryTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A primary view for the graph and whatnot"));
}

const FName FCadenceGraphPropertiesTabFactory::Identifier = FName(TEXT("FCadenceGraphDetailsTab"));

FCadenceGraphPropertiesTabFactory::FCadenceGraphPropertiesTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication)
: FWorkflowTabFactory(Identifier, InApplication)
{
	Application = InApplication;

	TabLabel = FText::FromString(TEXT("Properties"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the Properties view of a Cadence Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the Properties view."));
}

TSharedRef<SWidget> FCadenceGraphPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FCadenceGraphApplication> App = Application.Pin();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.NotifyHook = nullptr;
	}

	TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(nullptr);
	App->SetSelectedDetailsView(DetailsView);

	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					DetailsView.ToSharedRef()	
				];	
}

FText FCadenceGraphPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Cadence Graph Properties View"));
}