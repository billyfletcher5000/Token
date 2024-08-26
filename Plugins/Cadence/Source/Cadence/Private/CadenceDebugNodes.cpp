// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceDebugNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceDebugFloatNode::CreateInputPins()
{
	AddInputPin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());
}

bool UCadenceDebugFloatNode::Execute()
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	TSharedPtr<UCadenceVariableFloat> Variable = StaticCastSharedPtr<UCadenceVariableFloat>(Pin->GetVariable());
	ensure(Variable);
	
	ensure(GEngine);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, *FString::SanitizeFloat(Variable->GetValue()));
	
	return true;
}
