// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphRunner.h"

#include "CadenceContext.h"
#include "CadenceGraph.h"
#include "CadenceGraphNode.h"
#include "CadenceGraphNodePin.h"
#include "CadenceSubsystem.h"
#include "CadenceVariable.h"

void UCadenceGraphRunner::Init(UCadenceContext* InContext)
{
	Context = InContext;
	Context->Runner = this;
}

void UCadenceGraphRunner::Begin()
{	
	TArray<UCadenceGraphNode*> RootNodes = Context->Graph->GetRootExecNodes();

	for(UCadenceGraphNode* RootNode : RootNodes)
	{
		UCadenceGraphRunnerPathway* Pathway = NewObject<UCadenceGraphRunnerPathway>(this);
		Pathway->Init(Context, RootNode);
		ActivePathways.Add(Pathway);
	}
}

void UCadenceGraphRunner::Tick(const float& InDeltaSeconds)
{
	for(UCadenceGraphRunnerPathway* Pathway : ActivePathways)
	{
		Pathway->Tick(InDeltaSeconds);
	}

	for(UCadenceGraphRunnerPathway* EndedPathway : EndedPathways)
	{
		ActivePathways.Remove(EndedPathway);
	}

	EndedPathways.Empty();

	if(ActivePathways.Num() == 0)
		End();
}

void UCadenceGraphRunner::End()
{
	UCadenceSubsystem* CadenceSubsystem = GWorld->GetSubsystem<UCadenceSubsystem>();
	if(ensure(CadenceSubsystem))
	{
		CadenceSubsystem->NotifyGraphComplete(this);
	}
}

void UCadenceGraphRunner::NotifyPathwayEnded(UCadenceGraphRunnerPathway* InPathway)
{
	if(!EndedPathways.Contains(InPathway))
		EndedPathways.Add(InPathway);
}

void UCadenceGraphRunner::RequestAdditionalPathway(UCadenceGraphNode* InStartNode)
{	
	UCadenceGraphRunnerPathway* Pathway = NewObject<UCadenceGraphRunnerPathway>(this);
	Pathway->Init(Context, InStartNode);
	ActivePathways.Add(Pathway);
}

UCadenceContext* UCadenceGraphRunner::GetContext()
{
	Context->Runner = this;
	return Context;
}

void UCadenceGraphRunnerPathway::Init(UCadenceContext* InContext, UCadenceGraphNode* InStartNode)
{
	Context = InContext;
	CurrentNode = InStartNode;
}

void UCadenceGraphRunnerPathway::Tick(const float& InDeltaSeconds)
{
	UCadenceContext* CurrentContext = GetContext();
	CurrentContext->DeltaSeconds = InDeltaSeconds;
	
	if(ExecuteNode(CurrentNode, CurrentContext))
	{		
		TArray<UCadenceGraphNodePin*> ConnectedPins = CurrentNode->GetThenPin()->GetConnectedPins();

		if(ConnectedPins.Num() == 0)
		{
			End();
			return;
		}
		
		uint32 ProcessedPins = 0;
		for(UCadenceGraphNodePin* ConnectedPin : ConnectedPins)
		{			
			// Process execution logic, spawning more pathways if necessary			
			if(ProcessedPins == 0)
			{
				CurrentNode = ConnectedPin->GetParentNode();
			}
			else
			{				
				CurrentContext->Runner->RequestAdditionalPathway(ConnectedPin->GetParentNode());
			}
		}
	}
}

void UCadenceGraphRunnerPathway::End()
{
	Context->Runner->NotifyPathwayEnded(this);
}

UCadenceContext* UCadenceGraphRunnerPathway::GetContext()
{
	Context->Pathway = this;
	return Context;
}

bool UCadenceGraphRunnerPathway::ExecuteNode(UCadenceGraphNode* InNode, UCadenceContext* InContext)
{
	TArray<UCadenceGraphNodePin*> InputPins = InNode->GetInputPins();
	for(UCadenceGraphNodePin* InputPin : InputPins)
		ProcessVariableInputPin(InContext, InputPin);

	return InNode->Execute(InContext);
}

void UCadenceGraphRunnerPathway::ProcessVariableInputPin(UCadenceContext* InContext, UCadenceGraphNodePin* InPin)
{
	UCadenceGraphNode* ParentNode = InPin->GetParentNode();
	// If it's a pure node asking for a variable's value, it should already have been processed at that point
	if(InPin->IsExec() || ParentNode->IsPure())
		return;
	
	TArray<UCadenceGraphNode*> NodeStack;

	GatherPureNodesContributingToPin(InPin, NodeStack);

	for(auto Iter = NodeStack.rbegin(); Iter != NodeStack.rend(); ++Iter)
	{
		UCadenceGraphNode* Node = *Iter;
		Node->Execute(InContext);
		PropagateOutputPinsToInputPins(Node, NodeStack, ParentNode);
	}
}

void UCadenceGraphRunnerPathway::GatherPureNodesContributingToPin(UCadenceGraphNodePin* InPin, TArray<UCadenceGraphNode*>& InNodeStack)
{
	TArray<UCadenceGraphNodePin*> ConnectedOutputPins = InPin->GetConnectedPins();
	int32 ConnectedPinCount = ConnectedOutputPins.Num();	
	
	ensure(ConnectedPinCount <= 1);

	if(ConnectedPinCount == 0)
		return; // Use default
	
	UCadenceGraphNodePin* ConnectedOutputPin = ConnectedOutputPins[0];
	UCadenceGraphNode* ConnectedNode = ConnectedOutputPin->GetParentNode();
	if(ConnectedNode->IsPure())
	{
		InNodeStack.Add(ConnectedNode);

		TArray<UCadenceGraphNodePin*> InputPins = ConnectedNode->InputPins;
		for(UCadenceGraphNodePin* InputPin : InputPins)
		{			
			GatherPureNodesContributingToPin(InputPin, InNodeStack);
		}		
	}
}

void UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins(UCadenceGraphNode* InNode, const TArray<UCadenceGraphNode*>& InAllowedNodes, UCadenceGraphNode* InEndNode)
{
	TArray<UCadenceGraphNodePin*> OutputPins = InNode->GetOutputPins();
	for (UCadenceGraphNodePin* OutputPin : OutputPins)
	{
		if(OutputPin->IsExec())
			continue;
		
		TArray<UCadenceGraphNodePin*> ConnectedInputPins = OutputPin->GetConnectedPins();
		for (UCadenceGraphNodePin* ConnectedPin : ConnectedInputPins)
		{
			UCadenceGraphNode* ParentNode = ConnectedPin->GetParentNode();
			if(InEndNode == ParentNode || InAllowedNodes.Contains(ParentNode))
				ConnectedPin->GetVariable()->CopyValueFrom(OutputPin->GetVariable());
		}
	}
}
