// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceTriggerNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

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

bool UCadenceTriggerSequenceNode::Execute()
{
	return Super::Execute();
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

bool UCadenceQuantizedTimeTriggerNode::Execute()
{
	return Super::Execute();
}
