// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceConstantValueNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceConstantValueFloatNode::CreateOutputPins()
{	
	AddOutputPin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());
}

bool UCadenceConstantValueFloatNode::Execute()
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	TSharedPtr<UCadenceVariableFloat> Variable = StaticCastSharedPtr<UCadenceVariableFloat>(Pin->GetVariable());
	
	if(!Variable.IsValid())
	{
		Variable = MakeShareable(NewObject<UCadenceVariableFloat>(Pin, Pin->GetVariableClass()));
		Pin->SetVariable(Variable);
	}

	Variable->SetValue(Value);
	
	return true;
}
