// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceVariable.h"

#include "Cadence.h"
#include "CadenceAsset.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraphNode.h"

class FUObjectStringHelper
{
public:
	template<typename T>
	static T* SetFromString(const FString& InStringValue)
	{
		FString ObjectPathLocal = InStringValue;
		ConstructorHelpers::StripObjectClass(ObjectPathLocal);

		// If this is not a full object path it's a relative path so should be saved as a string
		if (FPackageName::IsValidObjectPath(ObjectPathLocal))
		{
			FSoftObjectPath AssetRef = ObjectPathLocal;
			// @todo: why are we resolving here? We should resolve explicitly 
			// during load or not at all
			if(!GCompilingBlueprint)
			{
				return Cast<T>(AssetRef.TryLoad());
			}
			else
			{
				return Cast<T>(AssetRef.ResolveObject());
			}
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	static FString ConvertToValueString(TObjectPtr<T> InValue)
	{
		if(InValue)
			return InValue.GetPath();
		
		return FString();
	}
};

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

bool UCadenceVariableArray::Equals(UCadenceVariable* OtherVariable)
{
	UCadenceVariableArray* CastedVariable = Cast<UCadenceVariableArray>(OtherVariable);
	if(ensure(CastedVariable))
	{
		TArray<UCadenceVariable*>& OtherArray = CastedVariable->Value;
		if(Value.Num() != OtherArray.Num())
			return false;

		for(int Index = 0; Index < Value.Num(); ++Index)
		{
			if(!Value[Index]->Equals(OtherArray[Index]))
				return false;
		}

		return true;
	}

	return false;
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

	OnValueChanged.Broadcast();
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

int32 UCadenceVariableArray::GetIndexOfElement(UCadenceVariable* InVariable) const
{
	int32 ItemIndex = INDEX_NONE;
	int32 ArraySize = GetSize();
	for(int32 Index = 0; Index < ArraySize; ++Index)
	{
		UCadenceVariable* ArrayElement = GetElement(Index);
		if(ensure(ArrayElement->IsA(InVariable->GetClass())))
		{
			if(ArrayElement->Equals(InVariable))
			{
				ItemIndex = Index;
				break;
			}
		}
	}

	return ItemIndex;
}

bool UCadenceVariableArray::ContainsElement(UCadenceVariable* InVariable) const
{
	return GetIndexOfElement(InVariable) != INDEX_NONE;
}

int32 UCadenceVariableArray::AddElement(UCadenceVariable* InVariable)
{
	if(ensure(InVariable->IsA(VariableClass)))
		return Value.Add(DuplicateObject(InVariable, this->GetOuter()));

	return INDEX_NONE;
}

int32 UCadenceVariableArray::RemoveElement(UCadenceVariable* InVariable)
{
	int32 RemovalCount = Value.RemoveAll([&InVariable] (UCadenceVariable* InArrayVariable)
	{
		return InArrayVariable->Equals(InVariable);
	});

	return RemovalCount;
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
	SetValue(FCString::Atoi(*InStringValue));
}

FString UCadenceVariableInt::ConvertToValueString() const
{
	return FString::FromInt(Value);
}

void UCadenceVariableFloat::SetFromString(const FString& InStringValue)
{
	SetValue(FCString::Atof(*InStringValue));
}

FString UCadenceVariableFloat::ConvertToValueString() const
{
	return FString::SanitizeFloat(Value);
}

void UCadenceVariableDouble::SetFromString(const FString& InStringValue)
{
	SetValue(FCString::Atod(*InStringValue));
}

FString UCadenceVariableDouble::ConvertToValueString() const
{
	return FString::SanitizeFloat(Value);
}

void UCadenceVariableBool::SetFromString(const FString& InStringValue)
{
	SetValue(InStringValue == "true");
}

FString UCadenceVariableBool::ConvertToValueString() const
{
	return Value ? TEXT("true") : TEXT("false");
}

void UCadenceVariableVector::SetFromString(const FString& InStringValue)
{
	FVector Temp;	
	Temp.InitFromString(InStringValue);
	SetValue(Temp);
}

FString UCadenceVariableVector::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableVector2D::SetFromString(const FString& InStringValue)
{
	FVector2D Temp;	
	Temp.InitFromString(InStringValue);
	SetValue(Temp);
}

FString UCadenceVariableVector2D::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableRotator::SetFromString(const FString& InStringValue)
{
	FRotator Temp;
	Temp.InitFromString(InStringValue);
	SetValue(Temp);
}

FString UCadenceVariableRotator::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableString::SetFromString(const FString& InStringValue)
{
	SetValue(InStringValue);
}

FString UCadenceVariableString::ConvertToValueString() const
{
	return Value;
}

void UCadenceVariableName::SetFromString(const FString& InStringValue)
{
	SetValue(FName(InStringValue));
}

FString UCadenceVariableName::ConvertToValueString() const
{
	return Value.ToString();
}

bool UCadenceVariableText::Equals(UCadenceVariable* OtherVariable)
{	
	UCadenceVariableText* CastedVariable = Cast<UCadenceVariableText>(OtherVariable);
	if(ensure(CastedVariable))
		return CastedVariable->GetValue().EqualTo(GetValue());

	return false;
}

void UCadenceVariableText::SetFromString(const FString& InStringValue)
{
	SetValue(FText::FromString(InStringValue));
}

FString UCadenceVariableText::ConvertToValueString() const
{
	return Value.ToString();
}

void UCadenceVariableUObject::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<UObject>(InStringValue));
}

FString UCadenceVariableUObject::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}

void UCadenceVariableEnum::SetFromString(const FString& InStringValue)
{
	if(IsValid(EnumType))
		SetValue(EnumType->GetValueByNameString(InStringValue));
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
	SetValue(CastedVariable->GetValue());

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

void UCadenceVariableCadenceAsset::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<UCadenceAsset>(InStringValue));
}

FString UCadenceVariableCadenceAsset::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}

void UCadenceVariableReactionGroup::SetFromString(const FString& InStringValue)
{
	SetValue(FUObjectStringHelper::SetFromString<UCadenceReactionGroup>(InStringValue));
}

FString UCadenceVariableReactionGroup::ConvertToValueString() const
{
	return FUObjectStringHelper::ConvertToValueString(Value);
}
