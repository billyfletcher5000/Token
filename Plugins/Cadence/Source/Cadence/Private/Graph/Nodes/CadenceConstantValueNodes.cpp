// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceConstantValueNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

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

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
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

	UCadenceVariableInt* Variable = Pin->GetVariable<UCadenceVariableInt>();
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

	//UCadenceVariableBool* Variable = Pin->GetVariable<UCadenceVariableBool>();
	//Variable->SetValue(Value);
	
	return true;
}
