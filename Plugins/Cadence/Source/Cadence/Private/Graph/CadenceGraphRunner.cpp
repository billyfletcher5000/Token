// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraphRunner.h"

#include "CadenceContext.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "CadenceSubsystem.h"
#include "Graph/CadenceVariable.h"

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
	GetContext()->ActorLifetimeManager->OnGraphEnded();
	
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

	ECadenceNodeExecuteResult Result = ExecuteNode(CurrentNode, CurrentContext);
	if(Result == ECadenceNodeExecuteResult::Failed)
	{
		End();
		return;
	}
	
	if(Result == ECadenceNodeExecuteResult::Complete)
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

ECadenceNodeExecuteResult UCadenceGraphRunnerPathway::ExecuteNode(UCadenceGraphNode* InNode, UCadenceContext* InContext)
{
	TArray<UCadenceGraphNodePin*> InputPins = InNode->GetInputPins();
	for(UCadenceGraphNodePin* InputPin : InputPins)
		ProcessVariableInputPin(InContext, InputPin);

	ECadenceNodeExecuteResult Result = InNode->Execute(InContext);

	if(Result == ECadenceNodeExecuteResult::Complete)
	{		
		PropagateOutputPinsToInputPins(InNode, InContext);
		ReleaseInputPinVariables(InNode, InContext);
	}

	return Result;
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
		PropagateOutputPinsToInputPins(Node, NodeStack, ParentNode, InContext);
		ReleaseInputPinVariables(Node, InContext);
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

void UCadenceGraphRunnerPathway::ReleaseInputPinVariables(UCadenceGraphNode* InNode, UCadenceContext* InContext)
{
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins = InNode->GetInputPins();

	for(UCadenceGraphNodePin* InputPin : InputPins)
	{
		if(InputPin->IsExec())
			continue;

		UCadenceVariable* Variable = InputPin->GetVariable();
		if(ensure(Variable))
		{
			UE_LOG(LogCadence, Log, TEXT("ReleaseInputPinVariables Pre-Unregister: %s - %s"), *InNode->GetName(), *InputPin->GetGUID().ToString());
			Variable->OnParentNodeReleased(InContext);
		}
	}
}

void UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins(UCadenceGraphNode* InNode, UCadenceContext* InContext)
{
	TArray<UCadenceGraphNodePin*> OutputPins = InNode->GetOutputPins();
	for (UCadenceGraphNodePin* OutputPin : OutputPins)
	{
		if(OutputPin->IsExec())
			continue;
		
		TArray<UCadenceGraphNodePin*> ConnectedInputPins = OutputPin->GetConnectedPins();
		for (UCadenceGraphNodePin* ConnectedPin : ConnectedInputPins)
		{			
			UE_LOG(LogCadence, Log, TEXT("PropagateOutputPinsToInputPins Pre-CopyValue: %s - %s"), *InNode->GetName(), *ConnectedPin->GetGUID().ToString());
			InContext->ParentNode = ConnectedPin->GetParentNode();
			ConnectedPin->GetVariable()->CopyValueFrom(OutputPin->GetVariable(), InContext);			
		}

		UE_LOG(LogCadence, Log, TEXT("PropagateOutputPinsToInputPins Pre-Unregister: %s - %s"), *InNode->GetName(), *OutputPin->GetGUID().ToString());
		OutputPin->GetVariable()->OnParentNodeReleased(InContext);
	}
}

void UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins(UCadenceGraphNode* InNode, const TArray<UCadenceGraphNode*>& InAllowedNodes, UCadenceGraphNode* InEndNode, UCadenceContext* InContext)
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
			{
				UE_LOG(LogCadence, Log, TEXT("PropagateOutputPinsToInputPins Pre-CopyValue: %s - %s"), *InNode->GetName(), *ConnectedPin->GetGUID().ToString());
				InContext->ParentNode = ConnectedPin->GetParentNode();
				ConnectedPin->GetVariable()->CopyValueFrom(OutputPin->GetVariable(), InContext);
			}
		}

		UE_LOG(LogCadence, Log, TEXT("PropagateOutputPinsToInputPins Pre-Unregister: %s - %s"), *InNode->GetName(), *OutputPin->GetGUID().ToString());
		OutputPin->GetVariable()->OnParentNodeReleased(InContext);
	}
}