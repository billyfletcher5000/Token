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

void UCadenceTriggerSequenceNode::AddUserInputPin()
{
	FString PinName = FCadencePinConstants::Pin_Trigger.ToString() + " " + GetNameForAdditionalPin(PinIndex).ToString();
	PinIndex++;	
	AddInputVariablePin(FName(PinName), UCadenceVariableTrigger::StaticClass());
}

void UCadenceTriggerSequenceNode::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{
	RemoveInputPin(Pin);
}

bool UCadenceTriggerSequenceNode::CanRemovePin(const UCadenceGraphNodePin* Pin) const
{
	return !Pin->IsExec() && InputPins.Contains(Pin) && InputPins.Num() > 2;
}
