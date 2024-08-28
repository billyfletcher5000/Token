// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{

	UCadenceGraphEditor* ParentEditorGraph = Cast<UCadenceGraphEditor>(ParentGraph);
	ensure(ParentEditorGraph);

	UCadenceGraph* RuntimeGraph = ParentEditorGraph->GetRuntimeGraph();
	ensure(RuntimeGraph);

	RuntimeGraph->Modify();
	UCadenceGraphNode* RuntimeNode = RuntimeGraph->CreateNode(RuntimeNodeType, Location);
	RuntimeGraph->AddNode(RuntimeNode);
	
	FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*ParentGraph);
	UCadenceGraphEditorNode* Node = NodeCreator.CreateNode(bSelectNewNode);
	Node->Construct(RuntimeNode);
	NodeCreator.Finalize();

	if (FromPin)
	{
		//ParentGraph->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(Node, true, true);	
	
	return Node;
}
