// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceFlowNodes.h"


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
