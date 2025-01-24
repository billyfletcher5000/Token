// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphApplication.h"

#include "CadenceAsset.h"
#include "CadenceGraphDetailsTab.h"
#include "Graph/CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorGridNode.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphEditorRerouteNode.h"
#include "CadenceGraphPropertyCustomization.h"
#include "CadenceGraphSchema.h"
#include "CadencePalette.h"
#include "CadenceSequencerSectionNameCustomization.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "IDetailsView.h"
#include "Algo/ForEach.h"
#include "Graph/Nodes/CadenceGridNodes.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "HAL/PlatformApplicationMisc.h"

const FName FCadenceGraphApplication::ToolkitFName = FName(TEXT("CadenceGraphApplication"));
const FText FCadenceGraphApplication::BaseToolkitName = FText::FromString(TEXT("CadenceGraphApplication"));
const FString FCadenceGraphApplication::WorldCentricTabPrefix = TEXT("CadenceGraphApplication");
const FLinearColor FCadenceGraphApplication::WorldCentricTabColorScale = FLinearColor(0.549f, 0.0f, 0.784f, 0.5f);
const FString FCadenceGraphApplication::DocumentationLink = TEXT("https://github.com/billyfletcher5000");
TMap<TWeakObjectPtr<UCadenceGraph>, TWeakObjectPtr<UCadenceGraphEditor>> FCadenceGraphApplication::GraphToEditorGraphCache = TMap<TWeakObjectPtr<UCadenceGraph>, TWeakObjectPtr<UCadenceGraphEditor>>();

void FCadenceGraphApplication::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FCadenceGraphApplication::InitEditor(const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost> InToolkitHost, UObject* InObject)
{	
	WorkingAsset = Cast<UCadenceAsset>(InObject);
	ensure(WorkingAsset);

	if (WorkingAsset->GetPrimaryGraph() == nullptr)
	{
		WorkingAsset->CreateGraph();
	}
	else
	{
		WorkingAsset->GetPrimaryGraph()->SetFlags(RF_Transactional);
	}

	PinTypesChangedDelegateHandle = WorkingAsset->GetPrimaryGraph()->OnPinTypesChanged.AddRaw(this, &FCadenceGraphApplication::Refresh);

	PreSaveDelegateHandle = WorkingAsset->OnPreSaveDelegate.AddRaw(this, &FCadenceGraphApplication::OnWorkingAssetPreSave);
	
	if(!WorkingGraphEditor)
	{
		if(GraphToEditorGraphCache.Contains(WorkingAsset->GetPrimaryGraph()))
			WorkingGraphEditor = GraphToEditorGraphCache[WorkingAsset->GetPrimaryGraph()].Get();

		if(!WorkingGraphEditor)
		{
			UEdGraph* CreatedGraph = FBlueprintEditorUtils::CreateNewGraph(WorkingAsset, NAME_None, UCadenceGraphEditor::StaticClass(), UCadenceGraphSchema::StaticClass());	
			WorkingGraphEditor = Cast<UCadenceGraphEditor>(CreatedGraph);
			
			GraphToEditorGraphCache.Add(WorkingAsset->GetPrimaryGraph(), WorkingGraphEditor);
		}
	}

	WorkingGraphEditor->SetFlags(RF_Transactional);
	
	ensure(WorkingGraphEditor);
	WorkingGraphEditor->SetRuntimeGraph(WorkingAsset->GetPrimaryGraph());

	UndoOrRedoPerformedDelegateHandle = WorkingGraphEditor->OnUndoOrRedoPerformed.AddRaw(this, &FCadenceGraphApplication::OnEditorGraphUndoRedo);

	DocumentManager = MakeShareable(new FDocumentTracker);	
	TSharedPtr<FCadenceGraphApplication> ThisPtr(SharedThis(this));
	DocumentManager->Initialize(ThisPtr);
	
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
	
	if(WorkingAsset)
	{
		if(UCadenceGraph* Graph = WorkingAsset->GetPrimaryGraph())
			Graph->OnPinTypesChanged.Remove(PinTypesChangedDelegateHandle);
		WorkingAsset->OnPreSaveDelegate.Remove(PreSaveDelegateHandle);
	}
	
	if(WorkingGraphEditor)
		WorkingGraphEditor->OnUndoOrRedoPerformed.Remove(UndoOrRedoPerformedDelegateHandle);
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

void FCadenceGraphApplication::SetGraphDetailsView(const TSharedPtr<IDetailsView>& InDetailsView)
{
	GraphDetailsView = InDetailsView;
}

bool FCadenceGraphApplication::InEditingMode() const
{
	// TODO: Update this if read only mode is ever added
	return true;
}

void FCadenceGraphApplication::ChangeVariableType(UCadenceVariable* InVar, const FEdGraphPinType& InNewPinType)
{
	bool ReselectVariable = false;
	if(SelectedDetailsView.Pin()->GetSelectedObjects().Contains(InVar))
		ReselectVariable = true;
	
	const UCadenceGraphSchema* Schema = Cast<UCadenceGraphSchema>(GetWorkingGraphEditor()->GetSchema());
	if(UCadenceVariable* NewVariable = Schema->ChangeVariableType(InVar, GetWorkingGraph(), GetWorkingGraphEditor(), InNewPinType))
	{
		LastUsedVariableClass = NewVariable->GetClass(); 
		Refresh();

		if(ReselectVariable)
			SelectedDetailsView.Pin()->SetObject(NewVariable, true);
	}
}

void FCadenceGraphApplication::Refresh()
{
	for (UEdGraphNode* Node : WorkingGraphEditor->Nodes)
	{
		UCadenceGraphEditorNode* CadenceNode = Cast<UCadenceGraphEditorNode>(Node);
		CadenceNode->UpdateRuntimePosition();
	}
	
	ReconstructEditorGraph();

	OnRefresh.Broadcast();
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

FSlateBrush const* FCadenceGraphApplication::GetVarIconAndColorFromVariable(const UCadenceVariable* Property, FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut)
{
	SecondaryBrushOut = nullptr;
	if (Property != nullptr)
	{
		const UCadenceGraphSchema* CadenceGraphSchema = GetDefault<UCadenceGraphSchema>();

		FEdGraphPinType PinType;
		if (CadenceGraphSchema->ConvertVariableToPinType(Property, PinType)) // use schema to get the color
		{
			return GetVarIconAndColorFromPinType(PinType, IconColorOut, SecondaryBrushOut, SecondaryColorOut);
		}
	}
	return FAppStyle::GetBrush(TEXT("Kismet.AllClasses.VariableIcon"));
}

FSlateBrush const* FCadenceGraphApplication::GetVarIconAndColorFromPinType(const FEdGraphPinType& PinType,
	FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut)
{
	const UCadenceGraphSchema* CadenceGraphSchema = GetDefault<UCadenceGraphSchema>();
	IconColorOut = CadenceGraphSchema->GetPinTypeColor(PinType);
	SecondaryBrushOut = FBlueprintEditorUtils::GetSecondaryIconFromPin(PinType);
	SecondaryColorOut = CadenceGraphSchema->GetSecondaryPinTypeColor(PinType);
	return FBlueprintEditorUtils::GetIconFromPin(PinType);
}

void FCadenceGraphApplication::ReconstructEditorGraph()
{
	WorkingGraphEditor->Nodes.Empty();
	
	UCadenceGraph* RuntimeGraph = WorkingAsset->GetPrimaryGraph();
	ensure(RuntimeGraph);

	const TArray<TObjectPtr<UCadenceGraphNode>>& RuntimeNodes = RuntimeGraph->GetNodes();

	// Create nodes first
	for(UCadenceGraphNode* RuntimeNode : RuntimeNodes)
	{
		UCadenceGraphEditorNode* Node = nullptr;
		
		if(RuntimeNode->IsReroute())
		{			
			FGraphNodeCreator<UCadenceGraphEditorRerouteNode> NodeCreator(*WorkingGraphEditor);
			Node = NodeCreator.CreateNode(false);
			Node->Construct(RuntimeNode);
			Node->SetFlags(RF_Transactional);
			NodeCreator.Finalize();
		}
		else if(RuntimeNode->GetClass()->ImplementsInterface(UCadenceGraphGridCommandProvider::StaticClass()))
		{
			FGraphNodeCreator<UCadenceGraphEditorGridNode> NodeCreator(*WorkingGraphEditor);
			Node = NodeCreator.CreateNode(false);
			Node->Construct(RuntimeNode);
			Node->SetFlags(RF_Transactional);
			NodeCreator.Finalize();
		}
		else
		{
			FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*WorkingGraphEditor);
			Node = NodeCreator.CreateNode(false);
			Node->Construct(RuntimeNode);
			Node->SetFlags(RF_Transactional);
			NodeCreator.Finalize();
		}		
        
        //WorkingGraphEditor->AddNode(Node, false, true);
	}

	// Connect nodes after
	for(UEdGraphNode* EdGraphNode : WorkingGraphEditor->Nodes)
	{
		UCadenceGraphEditorNode* CadenceEdNode = Cast<UCadenceGraphEditorNode>(EdGraphNode);
		CadenceEdNode->ReconstructConnections();
	}

	TSharedPtr<SGraphEditor> GraphEditor = SlateGraphEditor.Pin();
	GraphEditor->NotifyGraphChanged();
}

void FCadenceGraphApplication::OnWorkingAssetPreSave()
{
	for(UEdGraphNode* EdGraphNode : WorkingGraphEditor->Nodes)
	{
		UCadenceGraphEditorNode* CadenceEditorNode = Cast<UCadenceGraphEditorNode>(EdGraphNode);
		UE_LOG(LogCadenceEditor, Warning, TEXT("OnWorkingAssetPreSave"));
		CadenceEditorNode->UpdateRuntimePosition();
	}
}

void FCadenceGraphApplication::OnEditorGraphUndoRedo()
{
	
	TSharedPtr<SGraphEditor> GraphEditor = SlateGraphEditor.Pin();
	GraphEditor->NotifyGraphChanged();
	WorkingGraphEditor->NotifyGraphChanged();
	ReconstructEditorGraph();
}

void FCadenceGraphApplication::DeleteSelectedNodes() const
{
	const FScopedTransaction Transaction(*FCadenceEditorConstants::ContextIdentifier, FText::FromString(TEXT("Cadence: Delete Node(s)")), nullptr);
	TSharedPtr<SGraphEditor> SlateGraph = SlateGraphEditor.Pin();
	const FGraphPanelSelectionSet& SelectedNodes = SlateGraph->GetSelectedNodes();
	
	for (FGraphPanelSelectionSet::TConstIterator NodeIt( SelectedNodes ); NodeIt; ++NodeIt)
	{
		if (UCadenceGraphEditorNode* Node = Cast<UCadenceGraphEditorNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				if(UEdGraph* Graph = Node->GetGraph(); Graph != nullptr)
				{
					Graph->Modify();
				}

				UCadenceGraphNode* RuntimeNode = Node->GetRuntimeGraphNode();
				RuntimeNode->DestroyNode();

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
	CopySelectedNodes();
	DeleteSelectedNodes();
}

void FCadenceGraphApplication::CopySelectedNodes()
{
	if (SlateGraphEditor.IsValid())
	{
		// Whole process stole from PCG plugin
		TSharedPtr<SGraphEditor> SlateEditor = SlateGraphEditor.Pin();
		const FGraphPanelSelectionSet SelectedNodes = SlateEditor->GetSelectedNodes();

		//TODO: evaluate creating a clipboard object instead of ownership hack
		for (UObject* SelectedNode : SelectedNodes)
		{
			UEdGraphNode* GraphNode = CastChecked<UEdGraphNode>(SelectedNode);
			GraphNode->PrepareForCopying();
		}

		FString ExportedText;
		FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
		FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

		for (UObject* SelectedNode : SelectedNodes)
		{
			if (UCadenceGraphEditorNode* CadenceGraphNode = Cast<UCadenceGraphEditorNode>(SelectedNode))
			{
				CadenceGraphNode->PostCopy();
			}
		}
	}
}

void FCadenceGraphApplication::DuplicateSelectedNodes()
{
	CopySelectedNodes();
	PasteClipboardNodes();
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
	if (SlateGraphEditor.IsValid())
	{
		TSharedPtr<SGraphEditor> PinnedSlateGraphEditor = SlateGraphEditor.Pin();
		PasteClipboardNodesAtLocation(PinnedSlateGraphEditor->GetPasteLocation());
	}
}

void FCadenceGraphApplication::PasteClipboardNodesAtLocation(const FVector2D& InLocation)
{
	if (!SlateGraphEditor.IsValid() || !WorkingAsset || !WorkingAsset->GetPrimaryGraph())
	{
		return;
	}

	UCadenceGraph* RuntimeGraph = WorkingAsset->GetPrimaryGraph();
	TSharedPtr<SGraphEditor> GraphEditorWidget = SlateGraphEditor.Pin();

	const FScopedTransaction Transaction(*FCadenceEditorConstants::ContextIdentifier, FText::FromString(TEXT("Cadence Editor: Paste")), nullptr);
	WorkingGraphEditor->Modify();

	// Clear the selection set (newly pasted stuff will be selected)
	GraphEditorWidget->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(WorkingGraphEditor, TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	// Number of nodes used to calculate AvgNodePosition
	int32 AvgCount = 0;

	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		if (PastedNode)
		{
			AvgNodePosition.X += PastedNode->NodePosX;
			AvgNodePosition.Y += PastedNode->NodePosY;
			++AvgCount;
		}
	}

	if (AvgCount > 0)
	{
		float InvNumNodes = 1.0f / float(AvgCount);
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	TArray<UCadenceGraphNode*> PastedRuntimeNodes;

	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		GraphEditorWidget->SetNodeSelection(PastedNode, true);

		PastedNode->NodePosX = (PastedNode->NodePosX - AvgNodePosition.X) + InLocation.X;
		PastedNode->NodePosY = (PastedNode->NodePosY - AvgNodePosition.Y) + InLocation.Y;

		PastedNode->SnapToGrid(SNodePanel::GetSnapGridSize());

		PastedNode->CreateNewGuid();
		
		UCadenceGraphEditorNode* PastedGraphEditorNode = Cast<UCadenceGraphEditorNode>(PastedNode);
		if(!ensure(PastedGraphEditorNode))
			continue;
		
		UCadenceGraphNode* PastedRuntimeNode = PastedGraphEditorNode->GetRuntimeGraphNode();
		if(!ensure(PastedRuntimeNode))
			continue;;

		PastedRuntimeNodes.Add(PastedRuntimeNode);		
	}

	// Clean up any connections that aren't to other nodes within the pasted set
	for(UCadenceGraphNode* PastedRuntimeNode : PastedRuntimeNodes)
	{
		PastedRuntimeNode->GenerateGUID();

		auto DisconnectNonPastedPinsLambda = [&PastedRuntimeNodes](TObjectPtr<UCadenceGraphNodePin> Pin)
		{
			TArray<UCadenceGraphNodePin*> PinsToDisconnect;
			Pin->PruneConnections();
			TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins = Pin->GetConnectedPins();
			for(TObjectPtr<UCadenceGraphNodePin> ConnectedPin : ConnectedPins)
			{
				UCadenceGraphNode* ParentNode = ConnectedPin->GetParentNode();
				if(!IsValid(ParentNode) || !PastedRuntimeNodes.Contains(ConnectedPin->GetParentNode()))
					PinsToDisconnect.Add(ConnectedPin);
			}

			for(UCadenceGraphNodePin* PinToDisconnect : PinsToDisconnect)
				Pin->DisconnectPin(PinToDisconnect);	
		};

		TArray<TObjectPtr<UCadenceGraphNodePin>>& InputPins = PastedRuntimeNode->GetInputPins();
		TArray<TObjectPtr<UCadenceGraphNodePin>>& OutputPins = PastedRuntimeNode->GetOutputPins();

		Algo::ForEach(InputPins, DisconnectNonPastedPinsLambda);
		Algo::ForEach(OutputPins, DisconnectNonPastedPinsLambda);
		
		RuntimeGraph->AddNode(PastedRuntimeNode);
	}

	GraphEditorWidget->NotifyGraphChanged();
}

bool FCadenceGraphApplication::HasValidNodesInClipboard() const
{	
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(WorkingGraphEditor, ClipboardContent);
}

const FName FCadenceGraphApplicationMode::ModeName = FName(TEXT("CadenceGraphApplicationMode"));
const FName FCadenceGraphApplicationMode::LayoutName = FName(TEXT("CadenceGraphApplicationMode_Layout_v1"));

FCadenceGraphApplicationMode::FCadenceGraphApplicationMode(TSharedPtr<FCadenceGraphApplication> InApplication)
: FApplicationMode(ModeName)
{
	Application = InApplication;
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphPrimaryTabFactory(InApplication)));
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphNodeDetailsTabFactory(InApplication)));
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphDetailsTabFactory(InApplication)));
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphVariablesTabFactory(InApplication)));

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
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->Split
				(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(
							FCadenceGraphDetailsTabFactory::Identifier, ETabState::OpenedTab
						)	
				)
				->Split
				(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(
							FCadenceGraphVariablesTabFactory::Identifier, ETabState::OpenedTab
						)	
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.5f)
				->AddTab(
				FCadenceGraphPrimaryTabFactory::Identifier, ETabState::OpenedTab
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab(
				FCadenceGraphNodeDetailsTabFactory::Identifier, ETabState::OpenedTab
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

const FName FCadenceGraphNodeDetailsTabFactory::Identifier = FName(TEXT("FCadenceGraphNodeDetailsTab"));

FCadenceGraphNodeDetailsTabFactory::FCadenceGraphNodeDetailsTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication)
: FWorkflowTabFactory(Identifier, InApplication)
{
	Application = InApplication;

	TabLabel = FText::FromString(TEXT("Node Details"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the Node Details view of a Cadence Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the Node Details view."));
}

TSharedRef<SWidget> FCadenceGraphNodeDetailsTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
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

	DetailsView->RegisterInstancedCustomPropertyTypeLayout(FCadenceSectionName::StaticStruct()->GetFName(),
	FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FCadenceSequencerSectionNameSelectCustomization::MakeInstance, App->GetWorkingGraph() ) );
	DetailsView->RegisterInstancedCustomPropertyLayout(UCadenceVariable::StaticClass(),
						  FOnGetDetailCustomizationInstance::CreateStatic(&FCadenceVariableDetailCustomization::MakeInstance, FChangeVariableTypeDelegate::CreateSP(App.ToSharedRef(), &FCadenceGraphApplication::ChangeVariableType)));
	DetailsView->RegisterInstancedCustomPropertyLayout(UCadenceVariableArray::StaticClass(),
						  FOnGetDetailCustomizationInstance::CreateStatic(&FCadenceVariableArrayDetailCustomization::MakeInstance));

	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					DetailsView.ToSharedRef()	
				];		
}

FText FCadenceGraphNodeDetailsTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Cadence Graph Node Details View"));
}