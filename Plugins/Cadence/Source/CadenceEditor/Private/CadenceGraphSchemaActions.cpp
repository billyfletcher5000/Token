// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "CadenceGraphEditorNode.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UCadenceGraphEditorNode* Node = NewObject<UCadenceGraphEditorNode>(ParentGraph);

	ParentGraph->Modify();
	
	Node->SetFlags(RF_Transactional);
	Node->CreateNewGuid();
	
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;

	Node->AllocateDefaultPins();

	UEdGraphPin* InputPin = Node->CreatePin(
		EGPD_Input,
		TEXT("Inputs"),
		TEXT("Input A")
	);
	InputPin->PinType.PinValueType

	Node->CreatePin(
		EGPD_Output,
		TEXT("Outputs"),
		TEXT("Output A")
	);

	Node->CreatePin(
		EGPD_Output,
		TEXT("Outputs"),
		TEXT("Output B")
	);

	if (FromPin)
	{
		ParentGraph->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}
	
	ParentGraph->AddNode(Node, true, true);
	
	
	return Node;
}
