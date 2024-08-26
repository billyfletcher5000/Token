// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceDurationNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceQuantizedDurationNode::CreateOutputPins()
{
	AddOutputPin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

bool UCadenceQuantizedDurationNode::Execute()
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Duration);
	ensure(Pin);

	TSharedPtr<UCadenceVariableFloat> Variable = StaticCastSharedPtr<UCadenceVariableFloat>(Pin->GetVariable());
	
	if(!Variable.IsValid())
	{
		Variable = MakeShareable(NewObject<UCadenceVariableFloat>(Pin, Pin->GetVariableClass()));
		Pin->SetVariable(Variable);
	}

	Variable->SetValue(0.69f);
	
	return true;
}
