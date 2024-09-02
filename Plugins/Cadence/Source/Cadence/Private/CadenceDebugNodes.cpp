// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceDebugNodes.h"

#include "CadenceContext.h"
#include "CadenceGraphNodePin.h"
#include "CadenceGraphRunner.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceDebugIntNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
}

bool UCadenceDebugIntNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	UCadenceVariableInt* Variable = Pin->GetVariable<UCadenceVariableInt>();
	ensure(Variable);
	
	ensure(GEngine);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *FString::FromInt(Variable->GetValue()));
	
	return true;
}

void UCadenceDebugFloatNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());	
}

bool UCadenceDebugFloatNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
	ensure(Variable);
	
	ensure(GEngine);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, *FString::SanitizeFloat(Variable->GetValue()));
	
	return true;
}
