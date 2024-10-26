// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceFlowNodes.h"

#include "Graph/CadenceGraphRunner.h"
#include "Graph/Nodes/CadenceRerouteNodes.h"


void UCadenceBranchNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Condition, UCadenceVariableBool::StaticClass());
}

void UCadenceBranchNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputExecPin(FCadencePinConstants::Pin_True);
	AddOutputExecPin(FCadencePinConstants::Pin_False);
}

ECadenceNodeExecuteResult UCadenceBranchNode::Execute(UCadenceContext* InContext)
{
	return ECadenceNodeExecuteResult::Complete;
}

TArray<UCadenceGraphNodePin*> UCadenceBranchNode::GetActuatingOutputExecPins() const
{
	UCadenceGraphNodePin* ConditionPin = GetInputPin(FCadencePinConstants::Pin_Condition);
	UCadenceVariableBool* BoolVariabble = ConditionPin->GetVariable<UCadenceVariableBool>();
	
	return { BoolVariabble->GetValue() ? GetOutputPin(FCadencePinConstants::Pin_True) : GetOutputPin(FCadencePinConstants::Pin_False) };
}

void UCadenceForNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_FirstIndex, UCadenceVariableInt::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_LastIndex, UCadenceVariableInt::StaticClass());
}

void UCadenceForNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputExecPin(FCadencePinConstants::Pin_LoopBody);
	AddOutputVariablePin(FCadencePinConstants::Pin_Index, UCadenceVariableInt::StaticClass());
	AddOutputExecPin(FCadencePinConstants::Pin_Completed);
}

void UCadenceForNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext)
{		
	UCadenceGraphNodePin* LoopBodyPin = GetOutputPin(FCadencePinConstants::Pin_LoopBody);
	if(!LoopBodyPin->HasConnections())
		return;
	
	UCadenceGraphNodePin* FirstIndexPin = GetInputPin(FCadencePinConstants::Pin_FirstIndex);
	UCadenceGraphNodePin* LastIndexPin = GetInputPin(FCadencePinConstants::Pin_LastIndex);
	int32 FirstIndex = FirstIndexPin->GetVariable<UCadenceVariableInt>()->GetValue();
	int32 LastIndex = LastIndexPin->GetVariable<UCadenceVariableInt>()->GetValue();

	UCadenceGraphNodePin* OutIndexPin = GetOutputPin(FCadencePinConstants::Pin_Index);
	
	if(FirstIndex == LastIndex)
		return;

	for (UCadenceGraphNodePin* ConnectedPin : LoopBodyPin->GetConnectedPins())
	{	
		UCadenceForLoopRunner* Runner = UCadenceForLoopRunner::Create(
			FirstIndex,
			LastIndex,
			InContext->Runner,
			ConnectedPin->GetParentNode(),
			OutIndexPin,
			InContext->bProcessNodesImmediately,
			InContext->DeltaSeconds);
		InActionList.Add(Runner);	
	}
}

TArray<UCadenceGraphNodePin*> UCadenceForNode::GetActuatingOutputExecPins() const
{
	return { GetOutputPin(FCadencePinConstants::Pin_Completed) };
}

UCadenceForLoopRunner* UCadenceForLoopRunner::Create(const int32& InFirstIndex, const int32& InLastIndex,
	UCadenceGraphRunner* InGraphRunner, UCadenceGraphNode* InParentNode, UCadenceGraphNodePin* InIndexPin,
	const bool& bInExecuteImmediately, const float& InDeltaSeconds)
{
	UCadenceForLoopRunner* Runner = NewObject<UCadenceForLoopRunner>(InParentNode);
	Runner->FirstIndex = InFirstIndex;
	Runner->LastIndex = InLastIndex;
	Runner->Runner = InGraphRunner;
	Runner->ParentNode = InParentNode;
	Runner->IndexPin = InIndexPin;
	Runner->bExecuteImmediately = bInExecuteImmediately;
	Runner->LastDeltaSeconds = InDeltaSeconds;
	return Runner;
}

void UCadenceForLoopRunner::Init()
{
	CurrentIndex = FirstIndex;
	bHasOutstandingPathway = true;
	CreateNextPathway();	
}

bool UCadenceForLoopRunner::Tick(const float& InDeltaSeconds)
{
	LastDeltaSeconds = InDeltaSeconds;
	return !bHasOutstandingPathway;
}

void UCadenceForLoopRunner::OnPathwayEnded()
{
	if(CurrentIndex == LastIndex)
	{
		bHasOutstandingPathway = false;
		return;
	}
	
	int32 IncrementValue = FirstIndex < LastIndex ? 1 : -1;

	CurrentIndex += IncrementValue;
	CreateNextPathway();
}

void UCadenceForLoopRunner::CreateNextPathway()
{
	UCadenceVariableInt* IndexVariable = IndexPin->GetVariable<UCadenceVariableInt>();
	IndexVariable->SetValue(CurrentIndex);

	// Manual propagation
	// TODO: Make this work properly in the runner system if possible, cannot work out if this is
	//		 an inevitability in the overall design or a technical implementation issue
	TArray<UCadenceGraphNodePin*> EndPointPins;
	for (UCadenceGraphNodePin* ConnectedPin : IndexPin->GetConnectedPins())
	{
		if(ConnectedPin->GetParentNode()->IsReroute())
		{
			UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(ConnectedPin->GetParentNode());
			EndPointPins.Append(RerouteNode->GetRerouteOutputNodeConnectedInputPins());
		}
		else
		{
			EndPointPins.Add(ConnectedPin);
		}
	}

	for(UCadenceGraphNodePin* EndPointPin : EndPointPins)
	{
		UCadenceVariableInt* EndVariable = EndPointPin->GetVariable<UCadenceVariableInt>();
		if(EndVariable)
		{
			EndVariable->CopyValueFrom(IndexVariable);
		}
	}	
	
	Runner->RequestAdditionalPathway(ParentNode.Get(),
		bExecuteImmediately,
		LastDeltaSeconds,
		UCadenceGraphRunner::FOnAdditionalPathWayEndedDelegate::CreateUObject(this, &UCadenceForLoopRunner::OnPathwayEnded));
}
