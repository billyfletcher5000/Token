// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceFlowNodes.h"

#include "CadenceAsset.h"
#include "CadenceAssetInstance.h"
#include "CadenceSubsystem.h"
#include "Graph/CadenceGraph.h"
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

void UCadenceRunGraphNode_Base::CreateInputPins()
{
	Super::CreateInputPins();
	UpdateVariableInputPins();
}

void UCadenceRunGraphNode_Base::CreateOutputPins()
{
	Super::CreateOutputPins();
	UpdateVariableOutputPins();
}

void UCadenceRunGraphNode_Base::UpdateVariableInputPins()
{	
	UCadenceGraph* Graph = GetGraph();
	if(!Graph)
		return;
	
	TArray<TObjectPtr<UCadenceVariable>> InputVariables = Graph->GetInputVariables();

	auto RemoveNotInInputList = [&] (UCadenceGraphNodePin* Pin)
	{
		return VariableInputPins.Contains(Pin) && !InputVariables.ContainsByPredicate([&Pin] (UCadenceVariable* Variable)
		{
			return Variable->GetUserVariableName() == Pin->GetPinName();
		});
	};

	InputPins.RemoveAll(RemoveNotInInputList);

	VariableInputPins.Empty();

	for(UCadenceVariable* InputVariable : InputVariables)
	{
		VariableInputPins.Add(AddInputVariablePinUnique(InputVariable->GetUserVariableName(), InputVariable->GetClass()));
	}
}

void UCadenceRunGraphNode_Base::UpdateVariableOutputPins()
{
	UCadenceGraph* Graph = GetGraph();
	if(!Graph)
		return;
	
	TArray<TObjectPtr<UCadenceVariable>> OutputVariables = Graph->GetOutputVariables();

	auto RemoveNotInOutputList = [&] (UCadenceGraphNodePin* Pin)
	{
		return VariableOutputPins.Contains(Pin) && !OutputVariables.ContainsByPredicate([&Pin] (UCadenceVariable* Variable)
		{
			return Variable->GetUserVariableName() == Pin->GetPinName();
		});
	};

	OutputPins.RemoveAll(RemoveNotInOutputList);

	for(UCadenceVariable* OutputVariable : OutputVariables)
	{
		VariableOutputPins.Add(AddOutputVariablePinUnique(OutputVariable->GetUserVariableName(), OutputVariable->GetClass()));
	}
}

void UCadenceRunGraphNode_Base::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext)
{
	TArray<UCadenceVariable*> Variables;
	for(TWeakObjectPtr<UCadenceGraphNodePin> VariableInputPin : VariableInputPins)
	{
		UCadenceVariable* Variable = VariableInputPin->GetVariable();
		Variable->SetUserVariableName(VariableInputPin->GetPinName());
		Variables.Add(Variable);
	}
	
	InActionList.Add(UCadenceRunGraphTickable::Create(InContext, GetGraph(), Variables, this));
}

UCadenceRunGraphTickable* UCadenceRunGraphTickable::Create(UCadenceContext* InContext, UCadenceGraph* InTargetGraph, const TArray<UCadenceVariable*>& InInputVariables, UObject* InOuter)
{
	UCadenceRunGraphTickable* Action = NewObject<UCadenceRunGraphTickable>(InOuter);
	Action->Context = InContext;
	Action->TargetGraph = InTargetGraph;
	
	Action->InputVariables.Reserve(InInputVariables.Num());
	for(UCadenceVariable* Var : InInputVariables)
		Action->InputVariables.AddUnique(Var);
	
	return Action;		
}

void UCadenceRunGraphTickable::Init()
{
	UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
	Runner = Subsystem->CreateRunner(Context->AssetInstance, TargetGraph.Get());

	// Set initial values of public variables
	UCadenceContext* RunnerContext = Runner->GetContext();
	UCadenceGraph* RunnerGraph = RunnerContext->Graph;

	TArray<TObjectPtr<UCadenceVariable>> RunnerInputVariables = RunnerGraph->GetInputVariables();
	
	for(TWeakObjectPtr<UCadenceVariable>& Variable : InputVariables)
	{
		auto RunnerEquivalent = RunnerInputVariables.FindByPredicate([&Variable](UCadenceVariable* RunnerVar)
		{
			return RunnerVar->GetUserVariableName() == Variable->GetUserVariableName();
		});

		if(RunnerEquivalent)
		{
			(*RunnerEquivalent)->CopyValueFrom(Variable.Get());
		}
	}
	
	Context->AssetInstance->AddAdditionalRunner(Runner.Get());	
	Runner->Begin();
}

bool UCadenceRunGraphTickable::Tick(const float& InDeltaSeconds)
{
	if(Context->AssetInstance->IsAdditionalRunnerComplete(Runner.Get()))
		return true;
	
	return false;
}

void UCadenceRunGraphAssetNode::CreateInputPins()
{
	Super::CreateInputPins();
	
	if(ValueChangedVariable.IsValid())
	{
		ValueChangedVariable->OnValueChanged.RemoveDynamic(this, &UCadenceRunGraphAssetNode::UpdateVariableInputPins);
		ValueChangedVariable->OnValueChanged.RemoveDynamic(this, &UCadenceRunGraphAssetNode::UpdateVariableOutputPins);
	}
	
	UCadenceGraphNodePin* Pin = AddInputVariablePin(FCadencePinConstants::Pin_CadenceAsset, UCadenceVariableCadenceAsset::StaticClass(), 1);
	ValueChangedVariable = Pin->GetVariable();
	ValueChangedVariable->OnValueChanged.AddUniqueDynamic(this, &UCadenceRunGraphAssetNode::UpdateVariableInputPins);
	ValueChangedVariable->OnValueChanged.AddUniqueDynamic(this, &UCadenceRunGraphAssetNode::UpdateVariableOutputPins);
}

UCadenceGraph* UCadenceRunGraphAssetNode::GetGraph() const
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_CadenceAsset);
	if(!Pin)
		return nullptr;

	UCadenceVariableCadenceAsset* Variable = Pin->GetVariable<UCadenceVariableCadenceAsset>();
	if(!ensure(Variable))
		return nullptr;

	TObjectPtr<UCadenceAsset> Asset = Variable->GetValue();
	if(!Asset)
		return nullptr;

	UCadenceGraph* Graph = Asset->GetPrimaryGraph();
	if(!ensure(Graph))
		return nullptr;

	return Graph;
}
