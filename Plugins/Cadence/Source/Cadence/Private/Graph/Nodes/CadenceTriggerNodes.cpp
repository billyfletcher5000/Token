// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceTriggerNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceTriggerSequenceNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddUserInputPin();
	AddUserInputPin();
}

void UCadenceTriggerSequenceNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

bool UCadenceTriggerSequenceNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceTriggerSequenceNode::AddUserInputPin()
{
	FString PinName = FCadencePinConstants::Pin_Trigger.ToString() + " " + GetNameForAdditionalPin(PinIndex).ToString();
	PinIndex++;	
	return AddInputVariablePin(FName(PinName), UCadenceVariableTrigger::StaticClass());
}

bool UCadenceTriggerSequenceNode::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{
	return RemoveInputPin(Pin);
}

bool UCadenceTriggerSequenceNode::CanRemovePin(const UCadenceGraphNodePin* Pin) const
{
	return !Pin->IsExec() && InputPins.Contains(Pin) && InputPins.Num() > 2;
}

void UCadenceQuantizedTimeTriggerNode::CreateOutputPins()
{	
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

bool UCadenceQuantizedTimeTriggerNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}
