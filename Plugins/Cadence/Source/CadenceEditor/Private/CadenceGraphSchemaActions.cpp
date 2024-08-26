// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "CadenceGraphEditorNode.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UCadenceGraphEditorNode* Node = NewObject<UCadenceGraphEditorNode>(ParentGraph);

	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;

	Node->CreatePin(
		EGPD_Input,
		TEXT("Inputs"),
		TEXT("Input A")
	);

	Node->CreatePin(
		EGPD_Input,
		TEXT("Inputs"),
		TEXT("Input B")
	);

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

	ParentGraph->AddNode(Node, true, true);
	ParentGraph->Modify();
	
	return Node;
}
