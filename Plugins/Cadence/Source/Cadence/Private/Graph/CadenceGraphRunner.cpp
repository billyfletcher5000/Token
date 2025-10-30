// Copyright (C) Billy Fletcher 2025


#include "Graph/CadenceGraphRunner.h"

#include "Cadence.h"
#include "CadenceContext.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "CadenceSubsystem.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceVariable.h"
#include "Graph/Nodes/CadenceRerouteNodes.h"

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

	for(UCadenceGraphRunnerPathway* AddedPathway : AddedPathways)
	{
		ActivePathways.Add(AddedPathway);
	}

	AddedPathways.Empty();
	
	for(UCadenceGraphRunnerPathway* EndedPathway : EndedPathways)
	{
		ActivePathways.Remove(EndedPathway);
		
		if(PathwayToCallback.Contains(EndedPathway))
		{
			PathwayToCallback.Remove(EndedPathway);
		}
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

	if(PathwayToCallback.Contains(InPathway))
	{
		FOnAdditionalPathWayEndedDelegate Callback = MoveTemp(PathwayToCallback[InPathway]); 
		Callback.Execute();
		Callback.Unbind();
	}
}

void UCadenceGraphRunner::RequestAdditionalPathway(UCadenceGraphNode* InStartNode, const bool& bInExecuteImmediately, const float& InDeltaSeconds, FOnAdditionalPathWayEndedDelegate InOnEndedCallback)
{	
	UCadenceGraphRunnerPathway* Pathway = NewObject<UCadenceGraphRunnerPathway>(this);
	Pathway->Init(DuplicateObject<UCadenceContext>(Context, this), InStartNode);
	AddedPathways.Add(Pathway);

	if(InOnEndedCallback.IsBound())
		PathwayToCallback.Add(Pathway, MoveTemp(InOnEndedCallback));

	if(bInExecuteImmediately)
		Pathway->Tick(InDeltaSeconds);
}

UCadenceContext* UCadenceGraphRunner::GetContext()
{
	Context->Runner = this;
	return Context;
}

void UCadenceGraphRunnerPathway::Init(UCadenceContext* InContext, UCadenceGraphNode* InStartNode)
{
	Context = InContext;
	SetCurrentNode(GetContext(), InStartNode);
}

void UCadenceGraphRunnerPathway::Tick(const float& InDeltaSeconds)
{
	UCadenceContext* CurrentContext = GetContext();
	CurrentContext->DeltaSeconds = InDeltaSeconds;

	ExecuteCurrentNode(CurrentContext);
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

void UCadenceGraphRunnerPathway::ExecuteCurrentNode(UCadenceContext* InContext)
{
	UCadenceGraphNode* CurrentNodeAtStart = CurrentNode;
	ECadenceNodeExecuteResult Result = CurrentNode->Execute(InContext);
	if(Result == ECadenceNodeExecuteResult::Failed)
	{
		End();
		return;
	}
	
	if(Result == ECadenceNodeExecuteResult::Complete || Result == ECadenceNodeExecuteResult::AdditionalExecPinActuated)
	{
		bool bAnyPinActuated = false;
		UCadenceGraphNode* ActuatingNode = CurrentNode;
		TArray<UCadenceGraphNodePin*> ActuatingExecPins = CurrentNode->GetActuatingOutputExecPins();
		for(UCadenceGraphNodePin* ActuatingExecPin : ActuatingExecPins)
		{
			TArray<UCadenceGraphNodePin*> ConnectedPins = ActuatingExecPin->GetConnectedPins();

			if(ConnectedPins.Num() == 0)
			{
				continue;
			}

			TArray<UCadenceGraphNodePin*> NextNodePins;
		
			for(UCadenceGraphNodePin* ConnectedPin : ConnectedPins)
			{
				UCadenceGraphNode* Node = ConnectedPin->GetParentNode();
				if(Node->IsReroute())
				{
					UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(Node);
					NextNodePins.Append(RerouteNode->GetRerouteOutputNodeConnectedInputPins());
				}
				else
				{
					NextNodePins.Add(ConnectedPin);
				}
			}

			if(NextNodePins.Num() == 0)
			{
				continue;
			}

			bAnyPinActuated = true;
		
			for(int32 NodeIndex = 0; NodeIndex < NextNodePins.Num(); ++NodeIndex)
			{
				// Process execution logic, spawning more pathways if necessary			
				if(Result == ECadenceNodeExecuteResult::Complete && ActuatingNode == CurrentNode)
				{
					SetCurrentNode(InContext, NextNodePins[0]->GetParentNode());
				}
				else
				{
					// We early propagate here as it seems inadvisable to have new pathways backtrack for any reason
					PropagateOutputPinsToInputPins(CurrentNode, InContext);
					InContext->Runner->RequestAdditionalPathway(NextNodePins[NodeIndex]->GetParentNode(), Context->bProcessNodesImmediately, InContext->DeltaSeconds);
				}
			}
		}

		if(Result == ECadenceNodeExecuteResult::Complete)
		{
			if(!bAnyPinActuated)
				End();
			else if(Context->bProcessNodesImmediately && ActuatingNode != CurrentNode)
				ExecuteCurrentNode(InContext);
		}
	}
}

void UCadenceGraphRunnerPathway::SetCurrentNode(UCadenceContext* InContext, UCadenceGraphNode* InNode)
{
	if(CurrentNode != InNode)
	{
		if(CurrentNode)
		{
			PropagateOutputPinsToInputPins(CurrentNode, InContext);
			ReleaseInputPinVariables(CurrentNode, InContext);
		}
		
		CurrentNode = InNode;

		if(CurrentNode != nullptr)
		{
			TArray<UCadenceGraphNodePin*> InputPins = CurrentNode->GetInputPins();
			for(UCadenceGraphNodePin* InputPin : InputPins)
				ProcessVariableInputPin(InContext, InputPin);
		}
	}
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
	if(ConnectedNode->IsReroute())
	{
		UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceSimpleRerouteNode>(ConnectedNode);
		ensure(RerouteNode);

		ConnectedNode = RerouteNode->GetRerouteInputNode();

		if(ConnectedNode == nullptr)
			return;
	}	
	
	if(ConnectedNode->IsPure())
	{
		if(!ConnectedNode->IsReroute())
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
			UE_LOG(LogCadence, Log, TEXT("ReleaseInputPinVariables Pre-Unregister: %s - %s"), *InNode->GetDebugName(), *InputPin->GetGUID().ToString());
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
		
		TArray<UCadenceGraphNodePin*> InputPinsToPropagateTo;		
		TArray<UCadenceGraphNodePin*> ConnectedInputPins = OutputPin->GetConnectedPins();
		
		for (UCadenceGraphNodePin* ConnectedPin : ConnectedInputPins)
		{
			UCadenceGraphNode* ParentNode = ConnectedPin->GetParentNode();
			UCadenceGraphNode* Node = ConnectedPin->GetParentNode();
			
			if(Node->IsReroute())
			{
				UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(Node);
				ensure(RerouteNode);
				InputPinsToPropagateTo.Append(RerouteNode->GetRerouteOutputNodeConnectedInputPins());
			}
			else
			{
				InputPinsToPropagateTo.Add(ConnectedPin);
			}			
		}

		for (UCadenceGraphNodePin* InputPin : InputPinsToPropagateTo)
		{			
			UE_LOG(LogCadence, Log, TEXT("UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins Pre-CopyValue: %s - %s"), *InNode->GetDebugName(), *InputPin->GetGUID().ToString());
			UCadenceGraphNode* Node = InputPin->GetParentNode();
			InContext->ParentNode = Node;
			InputPin->GetVariable()->CopyValueFrom(OutputPin->GetVariable(), InContext);
		}

		UE_LOG(LogCadence, Log, TEXT("UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins Pre-Unregister: %s - %s"), *InNode->GetDebugName(), *OutputPin->GetGUID().ToString());
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
		
		TArray<UCadenceGraphNodePin*> InputPinsToPropagateTo;		
		TArray<UCadenceGraphNodePin*> ConnectedInputPins = OutputPin->GetConnectedPins();
		
		for (UCadenceGraphNodePin* ConnectedPin : ConnectedInputPins)
		{
			UCadenceGraphNode* ParentNode = ConnectedPin->GetParentNode();
			if(InEndNode == ParentNode || InAllowedNodes.Contains(ParentNode))
			{
				UCadenceGraphNode* Node = ConnectedPin->GetParentNode();
				
				if(Node->IsReroute())
				{
					UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(Node);
					ensure(RerouteNode);
					InputPinsToPropagateTo.Append(RerouteNode->GetRerouteOutputNodeConnectedInputPins());
				}
				else
				{
					InputPinsToPropagateTo.Add(ConnectedPin);
				}
			}
		}

		for (UCadenceGraphNodePin* InputPin : InputPinsToPropagateTo)
		{			
			UE_LOG(LogCadence, Log, TEXT("UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins Pre-CopyValue: %s - %s"), *InNode->GetDebugName(), *InputPin->GetGUID().ToString());
			UCadenceGraphNode* Node = InputPin->GetParentNode();
			InContext->ParentNode = Node;
			InputPin->GetVariable()->CopyValueFrom(OutputPin->GetVariable(), InContext);
		}

		UE_LOG(LogCadence, Log, TEXT("UCadenceGraphRunnerPathway::PropagateOutputPinsToInputPins Pre-Unregister: %s - %s"), *InNode->GetDebugName(), *OutputPin->GetGUID().ToString());
		OutputPin->GetVariable()->OnParentNodeReleased(InContext);
	}	
}