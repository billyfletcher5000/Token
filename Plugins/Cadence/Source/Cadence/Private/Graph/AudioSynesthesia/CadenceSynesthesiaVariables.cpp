// Copyright (C) Billy Fletcher 2025


#include "Graph/AudioSynesthesia/CadenceSynesthesiaVariables.h"

#include "Utility/UObjectStringHelper.h"

void UCadenceVariableLoudnessNRT::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<ULoudnessNRT>(InStringValue));
}

FString UCadenceVariableLoudnessNRT::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}

void UCadenceVariableConstantQNRT::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<UConstantQNRT>(InStringValue));
}

FString UCadenceVariableConstantQNRT::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}

void UCadenceVariableOnsetNRT::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<UOnsetNRT>(InStringValue));
}

FString UCadenceVariableOnsetNRT::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}
