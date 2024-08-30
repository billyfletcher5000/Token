// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditorNode.h"

#include "CadenceGraphAddPinInterface.h"
#include "CadenceGraphNodePin.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphUtility.h"
#include "CadenceVariable.h"
#include "GraphEditorSettings.h"
#include "Framework/Commands/GenericCommands.h"

void UCadenceGraphEditorNode::Construct(TObjectPtr<UCadenceGraphNode> InRuntimeGraphNode)
{
	RuntimeGraphNode = InRuntimeGraphNode;

	const FVector2D Position = RuntimeGraphNode->GetPosition();
	NodePosX = Position.X;
	NodePosY = Position.Y;

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
		const TArray<TObjectPtr<UCadenceGraphNodePin>>& ConnectedRuntimePins = RuntimePin->GetConnectedPins();

		for(UCadenceGraphNodePin* ConnectedRuntimePin : ConnectedRuntimePins)
		{
			UCadenceGraphNode* ConnectedRuntimePinParent = ConnectedRuntimePin->GetParentNode();
			UCadenceGraphEditorNode* ConnectedEditorNode = UCadenceGraphUtility::GetGraphEditorNodeForRuntimeNode(GetGraph(), ConnectedRuntimePinParent);
			if(ensure(ConnectedEditorNode))
			{
				UEdGraphPin* OtherPin = ConnectedEditorNode->FindPin(ConnectedRuntimePin->GetPinName(), EdPin->Direction == EEdGraphPinDirection::EGPD_Input ? EGPD_Output : EGPD_Input);
				EdPin->MakeLinkTo(OtherPin);
			}
		}
	}
}

void UCadenceGraphEditorNode::UpdateRuntimePosition()
{
	RuntimeGraphNode->SetPosition(NodePosX, NodePosY);
}

void UCadenceGraphEditorNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaGeneral", FText::FromString(TEXT("General")));
	Section.AddMenuEntry(FGenericCommands::Get().Delete);
	Section.AddMenuEntry(FGenericCommands::Get().Cut);
	Section.AddMenuEntry(FGenericCommands::Get().Copy);
	Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
	Section.AddMenuEntry(FGenericCommands::Get().Paste);

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

void UCadenceGraphEditorNode::CreatePinInternal(const EEdGraphPinDirection& InDirection, UCadenceGraphNodePin* InPin)
{
	UEdGraphPin* InputPin = CreatePin(
			InDirection,
			InPin->IsExec() ? UCadenceGraphSchema::PC_Exec : UCadenceGraphSchema::PC_Variable,
			InPin->GetPinName()
		);

	if(!InPin->IsExec())
	{
		UCadenceVariable* VariableDefault = InPin->GetVariableClass()->GetDefaultObject<UCadenceVariable>();
		InputPin->PinType.PinSubCategory = VariableDefault->GetPinSubCategory();
	}
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
	return ((AddPinNode && AddPinNode->CanAddPin()) ? EVisibility::Visible : EVisibility::Collapsed);
}

TSharedPtr<SGraphNode> FCadenceGraphEditorNodeFactory::CreateNodeWidget(UEdGraphNode* InNode)
{
	if(UCadenceGraphEditorNode* CadenceEditNode = Cast<UCadenceGraphEditorNode>(InNode))
	{
		if(CadenceEditNode->GetRuntimeGraphNode()->GetClass()->ImplementsInterface(UCadenceGraphAddPinInterface::StaticClass()))
		{
			return SNew(SGraphNodeUserAddablePins, CadenceEditNode);
		}
	}
	
	return FGraphNodeFactory::CreateNodeWidget(InNode);
}
