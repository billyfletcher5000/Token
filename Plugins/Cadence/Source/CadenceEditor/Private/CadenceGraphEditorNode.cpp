// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditorNode.h"

#include "CadenceGraphNodePin.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphUtility.h"
#include "CadenceVariable.h"
#include "Framework/Commands/GenericCommands.h"

void UCadenceGraphEditorNode::Construct(TObjectPtr<UCadenceGraphNode> InRuntimeGraphNode)
{
	RuntimeGraphNode = InRuntimeGraphNode;

	const FVector2D Position = RuntimeGraphNode->GetPosition();
	NodePosX = Position.X;
	NodePosY = Position.Y;

	for(UCadenceGraphNodePin* RuntimeInputPin : RuntimeGraphNode->GetInputPins())
	{
		UEdGraphPin* InputPin = CreatePin(
			EGPD_Input,
			RuntimeInputPin->IsExec() ? UCadenceGraphSchema::PC_Exec : UCadenceGraphSchema::PC_Variable,
			RuntimeInputPin->GetPinName()
		);

		if(!RuntimeInputPin->IsExec())
		{
			UCadenceVariable* VariableDefault = RuntimeInputPin->GetVariableClass()->GetDefaultObject<UCadenceVariable>();
			InputPin->PinType.PinSubCategory = VariableDefault->GetPinSubCategory();
		}
	}

	for(UCadenceGraphNodePin* RuntimeOutputPin : RuntimeGraphNode->GetOutputPins())
	{
		UEdGraphPin* InputPin = CreatePin(
			EGPD_Output,
			RuntimeOutputPin->IsExec() ? UCadenceGraphSchema::PC_Exec : UCadenceGraphSchema::PC_Variable,
			RuntimeOutputPin->GetPinName()
		);

		if(!RuntimeOutputPin->IsExec())
		{
			UCadenceVariable* VariableDefault = RuntimeOutputPin->GetVariableClass()->GetDefaultObject<UCadenceVariable>();
			InputPin->PinType.PinSubCategory = VariableDefault->GetPinSubCategory();
		}
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

void UCadenceGraphEditorNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaGeneral", FText::FromString(TEXT("General")));
	Section.AddMenuEntry(FGenericCommands::Get().Delete);
	Section.AddMenuEntry(FGenericCommands::Get().Cut);
	Section.AddMenuEntry(FGenericCommands::Get().Copy);
	Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
	Section.AddMenuEntry(FGenericCommands::Get().Paste);
}
