// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditorNode.h"

#include "CadenceGraphNodePin.h"
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
		UCadenceVariable* VariableDefault = RuntimeInputPin->GetVariableClass()->GetDefaultObject<UCadenceVariable>();
		UEdGraphPin* InputPin = CreatePin(
			EGPD_Input,
			TEXT("Inputs"),
			RuntimeInputPin->GetPinName()
		);

		InputPin->PinType.PinSubCategory = VariableDefault->GetPinSubCategory();
	}

	for(UCadenceGraphNodePin* RuntimeOutputPin : RuntimeGraphNode->GetOutputPins())
	{
		UCadenceVariable* VariableDefault = RuntimeOutputPin->GetVariableClass()->GetDefaultObject<UCadenceVariable>();
		UEdGraphPin* InputPin = CreatePin(
			EGPD_Output,
			TEXT("Outputs"),
			RuntimeOutputPin->GetPinName()
		);

		InputPin->PinType.PinSubCategory = VariableDefault->GetPinSubCategory();
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
