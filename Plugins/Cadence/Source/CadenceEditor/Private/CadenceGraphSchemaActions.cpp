// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{

	UCadenceGraphEditor* ParentEditorGraph = Cast<UCadenceGraphEditor>(ParentGraph);
	ParentEditorGraph->Modify();

	UCadenceGraph* RuntimeGraph = ParentEditorGraph->GetRuntimeGraph();
	UCadenceGraphNode* RuntimeNode = RuntimeGraph->CreateNode(RuntimeNodeType, Location);
	
	FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*ParentGraph);
	UCadenceGraphEditorNode* Node = NodeCreator.CreateNode(bSelectNewNode);
	Node->Construct(RuntimeNode);
	NodeCreator.Finalize();

	if (FromPin)
	{
		ParentGraph->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}
	
	ParentGraph->AddNode(Node, true, true);
	
	
	return Node;
}
