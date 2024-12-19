// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceVariable.h"

#include "Cadence.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraphNode.h"

UCadenceVariableArray* UCadenceVariableArray::Create(TSubclassOf<UCadenceVariable> InVariableClass, UObject* InOuter)
{
	UCadenceVariableArray* Array = NewObject<UCadenceVariableArray>(InOuter);
	Array->VariableClass = InVariableClass;
	return Array;
}

FName UCadenceVariableArray::GetPinCategory() const
{
	if(IsValid(VariableClass))
		return VariableClass->GetDefaultObject<UCadenceVariable>()->GetPinCategory();

	return FCadencePinCategoryConstants::PC_Wildcard;
}

FLinearColor UCadenceVariableArray::GetPinColor() const
{
	if(IsValid(VariableClass))
		return VariableClass->GetDefaultObject<UCadenceVariable>()->GetPinColor();

	return FCadenceVariableColorConstants::VC_Wildcard;
}

void UCadenceVariableArray::CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext)
{
	UCadenceVariableArray* CastedVariable = Cast<UCadenceVariableArray>(OtherVariable);
	if(ensure(CastedVariable))
		SetValue(CastedVariable->GetValue());
}

void UCadenceVariableArray::SetValue(const TArray<UCadenceVariable*>& InValue)
{
	Value.Empty();
	for(UCadenceVariable* Var : InValue)
	{
		if(!ensure(Var->IsA(VariableClass)))
			return;
		
		Value.Add(DuplicateObject(Var, this->GetOuter()));
	}
}

void UCadenceVariableArray::SetVariableClass(const TSubclassOf<UCadenceVariable>& InVariableClass)
{
	if(VariableClass != InVariableClass)
	{
		VariableClass = InVariableClass;
		Value.RemoveAll([&InVariableClass](UCadenceVariable* InVar) { return !InVar->IsA(InVariableClass.Get()); });
	}
}

UCadenceVariable* UCadenceVariableArray::GetElement(const int32& InElementIndex) const
{
	return Value[InElementIndex];
}

bool UCadenceVariableArray::AddElement(UCadenceVariable* InVariable)
{
	if(ensure(InVariable->IsA(VariableClass)))
	{
		Value.Add(DuplicateObject(InVariable, this->GetOuter()));
		return true;
	}

	return false;
}

bool UCadenceVariableArray::RemoveElement(const int32& InElementIndex)
{
	if(InElementIndex < 0 || InElementIndex >= Value.Num())
		return false;
	
	Value.RemoveAt(InElementIndex);
	return true;
}

void UCadenceVariableArray::EmptyElements()
{
	Value.Empty();
}

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

void UCadenceVariableString::SetFromString(const FString& InStringValue)
{
	Value = InStringValue;
}

FString UCadenceVariableString::ConvertToValueString() const
{
	return Value;
}

void UCadenceVariableName::SetFromString(const FString& InStringValue)
{
	Value = FName(InStringValue);
}

FString UCadenceVariableName::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableText::SetFromString(const FString& InStringValue)
{
	Value = FText::FromString(InStringValue);
}

FString UCadenceVariableText::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableEnum::SetFromString(const FString& InStringValue)
{
	if(IsValid(EnumType))
		Value = EnumType->GetValueByNameString(InStringValue);
}

FString UCadenceVariableEnum::ConvertToValueString() const
{
	return IsValid(EnumType) ? EnumType->GetNameByValue(Value).ToString() : FString();
}

UCadenceVariableQuantizationPeriod::UCadenceVariableQuantizationPeriod()
{
	EnumType = StaticEnum<EQuartzCommandQuantization>();
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