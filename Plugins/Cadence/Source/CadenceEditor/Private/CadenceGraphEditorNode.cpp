// Copyright (C) Billy Fletcher 2025


#include "CadenceGraphEditorNode.h"

#include "Graph/CadenceGraphAddPinInterface.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceVariable.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphPropertyCustomization.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphUtility.h"
#include "GraphEditorSettings.h"
#include "SGraphNodeKnot.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/Nodes/CadenceOperationNodes.h"

void UCadenceGraphEditorNode::Construct(TObjectPtr<UCadenceGraphNode> InRuntimeGraphNode)
{
	RuntimeGraphNode = InRuntimeGraphNode;

	const FVector2D Position = RuntimeGraphNode->GetGraphPosition();
	NodePosX = Position.X;
	NodePosY = Position.Y;

	RefreshPins();
}

void UCadenceGraphEditorNode::RefreshPins()
{
	for(UEdGraphPin* Pin : Pins)
	{
		Pin->BreakAllPinLinks(false, false);
	}
	
	Pins.Empty();
	
	for(UCadenceGraphNodePin* RuntimeInputPin : RuntimeGraphNode->GetInputPins())
	{		
		CreatePinInternal(EGPD_Input, RuntimeInputPin);
	}

	for(UCadenceGraphNodePin* RuntimeOutputPin : RuntimeGraphNode->GetOutputPins())
	{		
		CreatePinInternal(EGPD_Output, RuntimeOutputPin);
	}
}

void UCadenceGraphEditorNode::ReconstructConnections()
{	
	for(UEdGraphPin* EdPin : Pins)
	{
		UCadenceGraphNodePin* RuntimePin = EdPin->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeGraphNode->GetInputPin(EdPin->PinName) : RuntimeGraphNode->GetOutputPin(EdPin->PinName);
		RuntimePin->PruneConnections();
		const TArray<TObjectPtr<UCadenceGraphNodePin>>& ConnectedRuntimePins = RuntimePin->GetConnectedPins();
		
		for(UCadenceGraphNodePin* ConnectedRuntimePin : ConnectedRuntimePins)
		{
			UCadenceGraphNode* ConnectedRuntimePinParent = ConnectedRuntimePin->GetParentNode();
			if(ensure(ConnectedRuntimePin))
			{
				UCadenceGraphEditorNode* ConnectedEditorNode = UCadenceGraphUtility::GetGraphEditorNodeForRuntimeNode(GetGraph(), ConnectedRuntimePinParent);
				if(ensure(ConnectedEditorNode))
				{
					UEdGraphPin* OtherPin = ConnectedEditorNode->FindPin(ConnectedRuntimePin->GetPinName(), EdPin->Direction == EEdGraphPinDirection::EGPD_Input ? EGPD_Output : EGPD_Input);
					EdPin->MakeLinkTo(OtherPin);
				}
			}
		}
	}
}

void UCadenceGraphEditorNode::UpdateRuntimePosition()
{
	FVector2D RuntimePos = RuntimeGraphNode->GetGraphPosition();
	UE_LOG(LogCadenceEditor, Verbose, TEXT("UpdateRuntimePosition: Position: This: (%d, %d) Runtime: (%f, %f)"), NodePosX, NodePosY, RuntimePos.X, RuntimePos.Y);
	RuntimeGraphNode->SetGraphPosition(NodePosX, NodePosY);
}

void UCadenceGraphEditorNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaGeneral", FText::FromString(TEXT("General")));
	Section.AddMenuEntry(FGenericCommands::Get().Delete);
	Section.AddMenuEntry(FGenericCommands::Get().Cut);
	Section.AddMenuEntry(FGenericCommands::Get().Copy);
	Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
	Section.AddMenuEntry(FGenericCommands::Get().Paste);

	if(Context->Pin && IsValid(RuntimeGraphNode))
	{
		if(UCadenceOperationNode_Base* OperationNode = Cast<UCadenceOperationNode_Base>(RuntimeGraphNode))
		{
			static const FName ConvNodeName = FName("PromotableOperatorPinConvs");
			static const FText ConvNodeStr = FText::FromString(TEXT("Pin Conversions"));

			UCadenceGraphNodePin* RuntimePin = Context->Pin->Direction == EGPD_Input ? OperationNode->GetInputPin(Context->Pin->PinName) : OperationNode->GetInputPin(Context->Pin->PinName);
			if(IsValid(RuntimePin))
			{
				TSet<TSubclassOf<UCadenceVariable>> PossibleTypes;
				if(OperationNode->IsPinPrimary(RuntimePin))
					PossibleTypes = OperationNode->GetPrimaryAllowedTypes();
				else if(OperationNode->IsPinSecondary(RuntimePin))
					PossibleTypes = OperationNode->GetSecondaryAllowedTypes();
				else if(OperationNode->IsPinResult(RuntimePin))
					PossibleTypes = OperationNode->GetResultAllowedTypes();

				if(!(PossibleTypes.Num() == 1 && PossibleTypes.Contains(RuntimePin->GetVariableClass())))
				{
					FToolMenuSection& ConversionSection = Menu->AddSection(ConvNodeName, ConvNodeStr);
				
					// Add the options to the context menu
					for (TSubclassOf<UCadenceVariable>& PinType : PossibleTypes)
					{
						UCadenceVariable* TempInst = NewObject<UCadenceVariable>(GetTransientPackage(), PinType);
						FFormatNamedArguments Args;
						Args.Add(TEXT("NewPinType"), FText::FromName(TempInst->GetDisplayName()));

						FName DisplayName = TempInst->GetDisplayName();
						FString PinConversionName = FString::Printf(TEXT("To %s"), *DisplayName.ToString());

						const UCadenceGraphSchema* CadenceSchema = Cast<UCadenceGraphSchema>(Context->Graph->GetSchema());

						ConversionSection.AddMenuEntry(
							FName(PinConversionName),
							FText::FromString(PinConversionName),
							FText::Format(FText::FromString(TEXT("Convert this pin type to '{NewPinType}'")), Args),
							FSlateIcon(),
							FUIAction(
								FExecuteAction::CreateUObject(CadenceSchema, &UCadenceGraphSchema::ConvertPinType, RuntimePin, Context->Pin, PinType)
							)
						);
					}
				}
			}
		}
	}

	if (!Context->bIsDebugging)
	{
		if(ICadenceGraphAddPinInterface* AddPinInterface = Cast<ICadenceGraphAddPinInterface>(RuntimeGraphNode))
		{
			static FName NodeName = FName("AddPinInterfaceNode");
			FText SectionLabel = FText::FromString(TEXT("Configurable Pins"));
			if (Context->Pin != nullptr)
			{
				UCadenceGraphNodePin* CadencePin = RuntimeGraphNode->GetInputPin(Context->Pin->PinName);
				if(AddPinInterface->CanRemovePin(CadencePin))
				{
					FToolMenuSection& NewSection = Menu->AddSection(NodeName, SectionLabel);
					NewSection.AddMenuEntry(
						TEXT("RemovePin"),
						FText::FromString(TEXT("Remove pin")),
						FText::FromString(TEXT("Remove this input pin")),
						FSlateIcon(),
						FUIAction(
							FExecuteAction::CreateUObject(const_cast<UCadenceGraphEditorNode*>(this), &UCadenceGraphEditorNode::RemoveUserInputPin, CadencePin)
						)
					);
				}
			}
			else if(AddPinInterface->CanAddPin())
			{
				FToolMenuSection& NewSection = Menu->AddSection(NodeName, SectionLabel);
				NewSection.AddMenuEntry(
					"AddPin",
					FText::FromString(TEXT("Add pin")),
					FText::FromString(TEXT("Add another input pin")),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateUObject(const_cast<UCadenceGraphEditorNode*>(this), &UCadenceGraphEditorNode::AddUserInputPin)
					)
				);
			}
		}
	}
}

void UCadenceGraphEditorNode::PrepareForCopying()
{	
	if (RuntimeGraphNode)
	{
		// Temporarily take ownership of the RuntimeGraphNode, so that it is not deleted when cutting
		RuntimeGraphNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void UCadenceGraphEditorNode::PreEditUndo()
{
	Super::PreEditUndo();
	
	if(IsValid(RuntimeGraphNode))
	{
		FVector2D RuntimePos = RuntimeGraphNode->GetGraphPosition();
		UE_LOG(LogCadenceEditor, Verbose, TEXT("PreEditUndo: Position: This: (%d, %d) Runtime: (%f, %f)"), NodePosX, NodePosY, RuntimePos.X, RuntimePos.Y);
	}
}

void UCadenceGraphEditorNode::PostEditUndo()
{
	Super::PostEditUndo();
	if(IsValid(RuntimeGraphNode))
	{
		FVector2D RuntimePos = RuntimeGraphNode->GetGraphPosition();
		UE_LOG(LogCadenceEditor, Verbose, TEXT("PostEditUndo: Position: This: (%d, %d) Runtime: (%f, %f)"), NodePosX, NodePosY, RuntimePos.X, RuntimePos.Y);
		UpdateRuntimePosition();

		if(Pins.IsEmpty())
			Construct(RuntimeGraphNode);
	}
	UCadenceGraphEditor* EditorGraph = Cast<UCadenceGraphEditor>(GetGraph());
	EditorGraph->NotifyChildElementUndoOrRedo();
}

void UCadenceGraphEditorNode::ReconstructNode()
{
	RefreshPins();
	ReconstructConnections();
	GetGraph()->NotifyNodeChanged(this);
}

void UCadenceGraphEditorNode::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{	
	if(ICadenceGraphAddPinInterface* AddPinInterface = Cast<ICadenceGraphAddPinInterface>(RuntimeGraphNode))
	{
		if(AddPinInterface->RemoveUserInputPin(Pin))
		{
			for(UEdGraphPin* EdPin : Pins)
			{
				if(Pin->GetPinName() == EdPin->PinName)
				{
					RemovePin(EdPin);
					ReconstructNode();
					GetGraph()->NotifyNodeChanged(this);
					break;
				}
			}
		}
	}
}

void UCadenceGraphEditorNode::AddUserInputPin()
{	
	if(ICadenceGraphAddPinInterface* AddPinInterface = Cast<ICadenceGraphAddPinInterface>(RuntimeGraphNode))
	{
		UCadenceGraphNodePin* Pin = AddPinInterface->AddUserInputPin();
		CreatePinInternal(EGPD_Input, Pin);
		ReconstructNode();
		GetGraph()->NotifyNodeChanged(this);
	}
}

void UCadenceGraphEditorNode::PostCopy()
{	
	if (RuntimeGraphNode)
	{
		UCadenceGraphEditor* EditorGraph = CastChecked<UCadenceGraphEditor>(GetGraph());
		UCadenceGraph* RuntimeGraph = EditorGraph->GetRuntimeGraph();
		check(RuntimeGraph);
		RuntimeGraphNode->Rename(nullptr, RuntimeGraph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

UEdGraphPin* UCadenceGraphEditorNode::GetInputPinByName(const FName& InName)
{
	for(UEdGraphPin* Pin : Pins)
	{
		if(Pin->Direction == EGPD_Input && Pin->PinName == InName)
			return Pin;
	}

	return nullptr;
}

UEdGraphPin* UCadenceGraphEditorNode::GetOutputPinByName(const FName& InName)
{
	for(UEdGraphPin* Pin : Pins)
	{
		if(Pin->Direction == EGPD_Output && Pin->PinName == InName)
			return Pin;
	}

	return nullptr;
}

void UCadenceGraphEditorNode::CreatePinInternal(const EEdGraphPinDirection& InDirection, UCadenceGraphNodePin* InPin)
{
	FName PinCategory = NAME_None;
	FName PinSubCategory = NAME_None;
	EPinContainerType ContainerType = EPinContainerType::None;
	UObject* PinSubCategoryObject = nullptr;
	FString PinDefaultValue, PinAutogeneratedDefaultValue;
	
	if(InPin->IsExec())
	{
		PinCategory = UCadenceGraphSchema::PC_Exec;
	}
	else if(TSubclassOf<UCadenceVariable> VarClass = InPin->GetVariableClass())
	{
		UCadenceVariable* CDO = VarClass->GetDefaultObject<UCadenceVariable>();
		PinCategory = CDO->GetPinCategory();
		PinSubCategory = CDO->GetPinSubCategory();
		PinSubCategoryObject = CDO->GetPinSubCategoryObject();

		if(!InPin->IsDefaultDisabled() && CDO->SupportsDefault())
		{
			PinAutogeneratedDefaultValue = CDO->ConvertToValueString();
			
			if(UCadenceVariable* Variable = InPin->GetVariable())
			{
				PinDefaultValue = Variable->ConvertToValueString();
				PinSubCategoryObject = CDO->GetPinSubCategoryObject();
			}
		}

		if(CDO->IsArray())
		{
			ContainerType = EPinContainerType::Array;
			if(TSubclassOf<UCadenceVariable> SubVarClass = InPin->GetVariableSecondaryClass())
			{
				UCadenceVariable* SubCDO = SubVarClass->GetDefaultObject<UCadenceVariable>();
				PinCategory = SubCDO->GetPinCategory();
			}
		}
	}
	else
	{
		PinCategory = UCadenceGraphSchema::PC_Wildcard;
	}
	
	UEdGraphPin* EdPin = CreatePin(
			InDirection,
			PinCategory,
			PinSubCategory,
			PinSubCategoryObject,
			InPin->GetPinName()
		);


	EdPin->AutogeneratedDefaultValue = PinAutogeneratedDefaultValue;
	EdPin->DefaultValue = PinDefaultValue;
	EdPin->PinType.ContainerType = ContainerType;
}

void SGraphNodeUserAddablePins::Construct(const FArguments& InArgs, UCadenceGraphEditorNode* InNode)
{
	GraphNode = InNode;

	SetCursor( EMouseCursor::CardinalCross );

	UpdateGraphNode();
}

void SGraphNodeUserAddablePins::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	TSharedRef<SWidget> AddPinButton = AddPinButtonContent(
		NSLOCTEXT("SequencerNode", "UserAddableNodeAddPinButton", "Add pin"),
		NSLOCTEXT("SequencerNode", "UserAddableNodeAddPinButton_ToolTip", "Add new pin"));

	FMargin AddPinPadding = GetDefault<UGraphEditorSettings>()->GetOutputPinPadding();
	AddPinPadding.Top += 6.0f;

	OutputBox->AddSlot()
	.AutoHeight()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Right)
	.Padding(AddPinPadding)
	[
		AddPinButton
	];
}

FReply SGraphNodeUserAddablePins::OnAddPin()
{
	UCadenceGraphEditorNode* CadenceEditorNode = Cast<UCadenceGraphEditorNode>(GraphNode);
	ICadenceGraphAddPinInterface* AddPinNode = Cast<ICadenceGraphAddPinInterface>(CadenceEditorNode->GetRuntimeGraphNode());
	ensure(AddPinNode);
	if (AddPinNode && AddPinNode->CanAddPin())
	{
		FScopedTransaction Transaction(NSLOCTEXT("UserAddableNode", "AddPinTransaction", "Add Pin"));

		CadenceEditorNode->AddUserInputPin();
		GraphNode->GetGraph()->NotifyNodeChanged(GraphNode);
	}
	
	return FReply::Handled();
}

EVisibility SGraphNodeUserAddablePins::IsAddPinButtonVisible() const
{
	UCadenceGraphEditorNode* CadenceEditorNode = Cast<UCadenceGraphEditorNode>(GraphNode);
	ICadenceGraphAddPinInterface* AddPinNode = Cast<ICadenceGraphAddPinInterface>(CadenceEditorNode->GetRuntimeGraphNode());
	ensure(AddPinNode);
	return (AddPinNode && AddPinNode->CanAddPin()) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SCadenceGraphNode::Construct(const FArguments& InArgs, UCadenceGraphEditorNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor( EMouseCursor::CardinalCross );

	this->UpdateGraphNode();
}

void SCadenceGraphNode::MoveTo(const FVector2f& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	if(UCadenceGraphEditorNode* CadenceEdNode = Cast<UCadenceGraphEditorNode>(GraphNode))
		CadenceEdNode->UpdateRuntimePosition();
}

TSharedPtr<SGraphPin> SCadenceGraphNode::CreatePinWidget(UEdGraphPin* Pin) const
{
	TSharedPtr<SGraphPin> Widget = SGraphNode::CreatePinWidget(Pin);

	if(Widget.IsValid() && Pin)
	{		
		if(UCadenceGraphEditorNode* CadenceEdNode = Cast<UCadenceGraphEditorNode>(GraphNode))
		{
			if(UCadenceGraphNode* RuntimeNode = CadenceEdNode->GetRuntimeGraphNode())
			{
				if(UCadenceGraphNodePin* RuntimePin = Pin->Direction == EGPD_Input ? RuntimeNode->GetInputPin(Pin->PinName) : RuntimeNode->GetOutputPin(Pin->PinName))
				{
					Widget->SetShowLabel(!RuntimePin->GetShouldHidePinName());
				}
			}
		}
	}
	
	return Widget;
}

TSharedPtr<SGraphNode> FCadenceGraphEditorNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if(UCadenceGraphEditorNode* CadenceEditNode = Cast<UCadenceGraphEditorNode>(InNode))
	{
		if(CadenceEditNode->GetRuntimeGraphNode()->GetClass()->ImplementsInterface(UCadenceGraphAddPinInterface::StaticClass()))
		{
			return SNew(SGraphNodeUserAddablePins, CadenceEditNode);
		}
		else
		{
			return SNew(SCadenceGraphNode, CadenceEditNode);
		}
	}
	
	return FGraphPanelNodeFactory::CreateNode(InNode);
}
