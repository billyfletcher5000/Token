// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceDurationNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceQuantizedDurationNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

bool UCadenceQuantizedDurationNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Duration);
	ensure(Pin);

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
	Variable->SetValue(0.69f);
	
	return true;
}
