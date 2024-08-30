// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceConstantValueNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

FText UCadenceConstantValueFloatNode::GetNodeTitle() const
{
	return FText::FromString(FString::SanitizeFloat(Value));
}

void UCadenceConstantValueFloatNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());
}

bool UCadenceConstantValueFloatNode::Execute(UCadenceContext* InContext)
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

FText UCadenceConstantValueIntNode::GetNodeTitle() const
{
	return FText::FromString(FString::FromInt(Value));
}

void UCadenceConstantValueIntNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
}

bool UCadenceConstantValueIntNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Int);
	ensure(Pin);

	TSharedPtr<UCadenceVariableInt> Variable = StaticCastSharedPtr<UCadenceVariableInt>(Pin->GetVariable());
	
	if(!Variable.IsValid())
	{
		Variable = MakeShareable(NewObject<UCadenceVariableInt>(Pin, Pin->GetVariableClass()));
		Pin->SetVariable(Variable);
	}

	Variable->SetValue(Value);
	
	return true;
}


FText UCadenceConstantValueBoolNode::GetNodeTitle() const
{
	return Value ? FText::FromString(TEXT("True")) : FText::FromString(TEXT("False"));
}

void UCadenceConstantValueBoolNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Bool, UCadenceVariableBool::StaticClass());
}

bool UCadenceConstantValueBoolNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Bool);
	ensure(Pin);

	TSharedPtr<UCadenceVariableBool> Variable = StaticCastSharedPtr<UCadenceVariableBool>(Pin->GetVariable());
	
	if(!Variable.IsValid())
	{
		Variable = MakeShareable(NewObject<UCadenceVariableBool>(Pin, Pin->GetVariableClass()));
		Pin->SetVariable(Variable);
	}

	Variable->SetValue(Value);
	
	return true;
}
