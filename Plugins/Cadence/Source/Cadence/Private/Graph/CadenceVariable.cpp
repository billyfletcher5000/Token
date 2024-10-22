// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceVariable.h"
#include "Graph/CadenceGraphNode.h"

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

void UCadenceVariableEnum::SetFromString(const FString& InStringValue)
{
	Value = EnumType->GetValueByNameString(InStringValue);
}

FString UCadenceVariableEnum::ConvertToValueString() const
{
	return EnumType->GetNameByValue(Value).ToString();
}

void UCadenceVariableActor::CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext)
{
	UCadenceVariableActor* CastedVariable = Cast<UCadenceVariableActor>(OtherVariable);
	Value = CastedVariable->GetValue();

	if(InContext)
	{
		FGuid TempGUID = GetGUID();
		UE_LOG(LogCadence, Log, TEXT("CopyValueFrom Register: %s - This - %s - Other - %s - Node - %s"), *GetName(), *TempGUID.ToString(), *OtherVariable->GetGUID().ToString(), *InContext->ParentNode->GetName());
		InContext->ActorLifetimeManager->RegisterActorUsage(Value, TempGUID, InContext->ParentNode);
	}	
}

void UCadenceVariableActor::OnParentNodeReleased(UCadenceContext* InContext)
{
	if(ensure(InContext))
	{
		FGuid TempGUID = GetGUID();
		UE_LOG(LogCadence, Log, TEXT("OnParentNodeReleased Unregister: %s - %s"), *GetName(), *TempGUID.ToString());
		InContext->ActorLifetimeManager->UnregisterActorUsage(Value, TempGUID);
	}
}