// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceVariable.h"

void UCadenceVariableInt::SetFromString(const FString& InStringValue)
{
	Value = FCString::Atoi(*InStringValue);
}

FString UCadenceVariableInt::ConvertToValueString() const
{
	return FString::FromInt(Value);
}

void UCadenceVariableFloat::SetFromString(const FString& InStringValue)
{
	Value = FCString::Atof(*InStringValue);
}

FString UCadenceVariableFloat::ConvertToValueString() const
{
	return FString::SanitizeFloat(Value);
}

void UCadenceVariableDouble::SetFromString(const FString& InStringValue)
{
	Value = FCString::Atod(*InStringValue);
}

FString UCadenceVariableDouble::ConvertToValueString() const
{
	return FString::SanitizeFloat(Value);
}

void UCadenceVariableBool::SetFromString(const FString& InStringValue)
{
	Value = InStringValue == "true";
}

FString UCadenceVariableBool::ConvertToValueString() const
{
	return Value ? TEXT("true") : TEXT("false");
}

void UCadenceVariableVector::SetFromString(const FString& InStringValue)
{
	Value.InitFromString(InStringValue);
}

FString UCadenceVariableVector::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableVector2D::SetFromString(const FString& InStringValue)
{
	Value.InitFromString(InStringValue);
}

FString UCadenceVariableVector2D::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableRotator::SetFromString(const FString& InStringValue)
{
	Value.InitFromString(InStringValue);
}

FString UCadenceVariableRotator::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableQuartzCommandQuantization::SetFromString(const FString& InStringValue)
{
	Value = (EQuartzCommandQuantization)StaticEnum<EQuartzCommandQuantization>()->GetValueByNameString(InStringValue);
}

FString UCadenceVariableQuartzCommandQuantization::ConvertToValueString() const
{
	return StaticEnum<EQuartzCommandQuantization>()->GetValueAsString(Value);
}
