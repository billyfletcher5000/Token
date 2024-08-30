// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceMathNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

// TODO: Add support for base numericals rather than just floats

void UCadenceAddNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddUserInputPin();
	AddUserInputPin();
}

void UCadenceAddNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());
}

bool UCadenceAddNode::Execute(UCadenceContext* InContext)
{
	float AggregateValue = 0.0f;

	for(UCadenceGraphNodePin* InputPin : InputPins)
	{
		UCadenceVariableFloat* FloatVariable = Cast<UCadenceVariableFloat>(InputPin->GetVariable().Get());
		AggregateValue += FloatVariable->GetValue();
	}

	UCadenceVariableFloat* OutputFloatVariable = Cast<UCadenceVariableFloat>(OutputPins[0]->GetVariable().Get());
	OutputFloatVariable->SetValue(AggregateValue);
	
	return true;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceAddNode::AddUserInputPin()
{
	FString PinName = FCadencePinConstants::Pin_Float.ToString() + " " + GetNameForAdditionalPin(PinIndex).ToString();
	PinIndex++;	
	return AddInputVariablePin(FName(PinName), UCadenceVariableFloat::StaticClass());
}

bool UCadenceAddNode::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{
	return RemoveInputPin(Pin);
}

bool UCadenceAddNode::CanRemovePin(const UCadenceGraphNodePin* Pin) const
{
	return !Pin->IsExec() && InputPins.Contains(Pin) && InputPins.Num() > 2;
}
