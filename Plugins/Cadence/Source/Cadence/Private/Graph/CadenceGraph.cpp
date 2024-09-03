// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraph.h"

#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"

TObjectPtr<UCadenceGraphNode> UCadenceGraph::CreateNode(TSubclassOf<UCadenceGraphNode> InClass,	const FVector2D& Location)
{
	TObjectPtr<UCadenceGraphNode> Node = NewObject<UCadenceGraphNode>(this, InClass);

	Node->SetFlags(RF_Transactional);	
	
	Node->SetParentGraph(this);
	Node->SetPosition(Location);
	
	return Node;
}

void UCadenceGraph::AddNode(TObjectPtr<UCadenceGraphNode> InNode)
{
	Nodes.Add(InNode);
}

void UCadenceGraph::RemoveNode(TObjectPtr<UCadenceGraphNode> InNode)
{
	Modify();
	Nodes.Remove(InNode);
}

void UCadenceGraph::ClearNodes()
{
	Nodes.Empty();
}

TArray<TObjectPtr<UCadenceGraphNode>> UCadenceGraph::GetRootExecNodes() const
{
	TArray<TObjectPtr<UCadenceGraphNode>> Result;
	
	for(auto Iter = Nodes.begin(); Iter != Nodes.end(); ++Iter)
	{
		UCadenceGraphNode* Node = (*Iter);

		if(Node->IsPure())
			continue;
					
		UCadenceGraphNodePin* ExecPin = Node->GetExecPin();

		if(ExecPin == nullptr || ExecPin->GetConnectedPins().Num() == 0)
		{
			Result.Add(Node);
		}		
	}

	return Result;
}
