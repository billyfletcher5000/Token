// Copyright (C) Billy Fletcher 2025


#include "Graph/Nodes/CadenceConstantValueNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

FText UCadenceConstantValueIntNode::GetNodeTitle() const
{
	return FText::FromString(FString::FromInt(Value));
}

void UCadenceConstantValueIntNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueIntNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Int);
	ensure(Pin);

	UCadenceVariableInt* Variable = Pin->GetVariable<UCadenceVariableInt>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueFloatNode::GetNodeTitle() const
{
	return FText::FromString(FString::SanitizeFloat(Value));
}

void UCadenceConstantValueFloatNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueFloatNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceConstantValueStringNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_String, UCadenceVariableString::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueStringNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_String);
	ensure(Pin);

	UCadenceVariableString* Variable = Pin->GetVariable<UCadenceVariableString>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueStringNode::GetNodeTitle() const
{
	return FText::FromString(Value);
}

void UCadenceConstantValueStringArrayNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	UCadenceGraphNodePin* Pin = AddOutputVariablePin(FCadencePinConstants::Pin_Array, UCadenceVariableArray::StaticClass());
	
	UCadenceVariableArray* ArrayVariable = UCadenceVariableArray::Create(UCadenceVariableString::StaticClass(), Pin);
	Pin->SetVariable(ArrayVariable);
}

ECadenceNodeExecuteResult UCadenceConstantValueStringArrayNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Array);
	ensure(Pin);

	UCadenceVariableArray* ArrayVariable = Pin->GetVariable<UCadenceVariableArray>(false);
	ensure(ArrayVariable);

	ArrayVariable->EmptyElements();

	for(FString& String : Value)
	{
		UCadenceVariableString* StringVariable = NewObject<UCadenceVariableString>(ArrayVariable);
		StringVariable->SetValue(String);
		ArrayVariable->AddElement(StringVariable);
	}
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueStringArrayNode::GetNodeTitle() const
{
	return FText::FromString("StrArray");
}

void UCadenceConstantValueBoolNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Bool, UCadenceVariableBool::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueBoolNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Bool);
	ensure(Pin);

	UCadenceVariableBool* Variable = Pin->GetVariable<UCadenceVariableBool>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueBoolNode::GetNodeTitle() const
{
	return Value ? FText::FromString(TEXT("True")) : FText::FromString(TEXT("False"));
}

void UCadenceConstantValueVectorNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Vector, UCadenceVariableVector::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueVectorNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Vector);
	ensure(Pin);

	UCadenceVariableVector* Variable = Pin->GetVariable<UCadenceVariableVector>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueVectorNode::GetNodeTitle() const
{
	return Value.ToText();
}

void UCadenceConstantValueVector2Node::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Vector2, UCadenceVariableVector2D::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueVector2Node::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Vector2);
	ensure(Pin);

	UCadenceVariableVector2D* Variable = Pin->GetVariable<UCadenceVariableVector2D>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueVector2Node::GetNodeTitle() const
{
	return FText::FromString(Value.ToString());
}

void UCadenceConstantValueQuantizationPeriodNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Period, UCadenceVariableQuantizationPeriod::StaticClass());
}

ECadenceNodeExecuteResult UCadenceConstantValueQuantizationPeriodNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Period);
	ensure(Pin);

	UCadenceVariableQuantizationPeriod* Variable = Pin->GetVariable<UCadenceVariableQuantizationPeriod>();
	Variable->SetValue(Value);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceConstantValueQuantizationPeriodNode::GetNodeTitle() const
{
	FString Title = StaticEnum<EQuartzCommandQuantization>()->GetValueAsString(Value);
	Title.RemoveFromStart(TEXT("EQuartzCommandQuantization::"));
	return FText::FromString(Title);
}
