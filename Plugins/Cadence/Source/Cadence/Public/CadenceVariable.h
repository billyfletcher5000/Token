// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceVariable.generated.h"

class UCadenceTriggerData;

namespace FCadenceVariable
{
	
}

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceVariable : public UObject
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const PURE_VIRTUAL(UCadenceVariable::GetPinSubCategory, return NAME_None;);
	virtual FLinearColor GetPinColor() const PURE_VIRTUAL(UCadenceVariable::GetPinSubCategory, return FLinearColor::White;);

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) PURE_VIRTUAL();

	virtual bool IsArray() const { return false; }
	virtual bool IsEnum() const  { return false; }
};


UCLASS()
class CADENCE_API UCadenceVariableInt : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("int32"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.4f, 0.4f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableInt* IntVariable = Cast<UCadenceVariableInt>(OtherVariable);
		Value = IntVariable->GetValue();
	}

	int32 GetValue() const { return Value; }
	void SetValue(const int32& InValue) { Value = InValue; }

private:
	UPROPERTY()
	int32 Value;
};


UCLASS()
class CADENCE_API UCadenceVariableFloat : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("float"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.0f, 1.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableFloat* CastedVariable = Cast<UCadenceVariableFloat>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	float GetValue() const { return Value; }
	void SetValue(const float& InValue) { Value = InValue; }

private:
	UPROPERTY()
	float Value;
};


UCLASS()
class CADENCE_API UCadenceVariableDouble : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("double"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.4f, 0.4f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableDouble* CastedVariable = Cast<UCadenceVariableDouble>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	double GetValue() const { return Value; }
	void SetValue(const double& InValue) { Value = InValue; }

private:
	UPROPERTY()
	double Value;
};

UCLASS()
class CADENCE_API UCadenceVariableBool : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("bool"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(1.0f, 0.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableBool* CastedVariable = Cast<UCadenceVariableBool>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	bool GetValue() const { return Value; }
	void SetValue(const bool& InValue) { Value = InValue; }

private:
	UPROPERTY()
	bool Value;
};

UCLASS()
class CADENCE_API UCadenceVariableVector : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("Vector"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(1.0f, 1.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableVector* CastedVariable = Cast<UCadenceVariableVector>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	FVector GetValue() const { return Value; }
	void SetValue(const FVector& InValue) { Value = InValue; }

private:
	UPROPERTY()
	FVector Value;
};

UCLASS()
class CADENCE_API UCadenceVariableVector2D : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("Vector2D"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.0f, 0.0f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableVector2D* CastedVariable = Cast<UCadenceVariableVector2D>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	FVector2D GetValue() const { return Value; }
	void SetValue(const FVector2D& InValue) { Value = InValue; }

private:
	UPROPERTY()
	FVector2D Value;
};

UCLASS()
class CADENCE_API UCadenceVariableUObject : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("UObject"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableUObject* CastedVariable = Cast<UCadenceVariableUObject>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	TObjectPtr<UObject> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UObject>& InValue) { Value = InValue; }

private:
	UPROPERTY()
	TObjectPtr<UObject> Value;
};


UCLASS()
class CADENCE_API UCadenceVariableUObjectArray : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("UObject"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableUObjectArray* CastedVariable = Cast<UCadenceVariableUObjectArray>(OtherVariable);
		Value = CastedVariable->GetValue();
	}

	virtual bool IsArray() const override { return true; }	
	
	TArray<TObjectPtr<UObject>> GetValue() const { return Value; }
	void SetValue(const TArray<TObjectPtr<UObject>>& InValue) { Value = InValue; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UObject>> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableQuartzCommandQuantization : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("QuartzCommandQuantization"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableQuartzCommandQuantization* CastedVariable = Cast<UCadenceVariableQuartzCommandQuantization>(OtherVariable);
		Value = CastedVariable->GetValue();
	}

	virtual bool IsEnum() const override { return true; }
	
	EQuartzCommandQuantization GetValue() const { return Value; }
	void SetValue(const EQuartzCommandQuantization& InValue) { Value = InValue; }

private:
	UPROPERTY()
	EQuartzCommandQuantization Value;
};

UCLASS()
class CADENCE_API UCadenceVariableTrigger : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinSubCategory() const override { return TEXT("Trigger"); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.75f, 0.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable) override
	{
		UCadenceVariableTrigger* CastedVariable = Cast<UCadenceVariableTrigger>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	TObjectPtr<UCadenceTriggerData> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UCadenceTriggerData>& InValue) { Value = InValue; }

private:
	UPROPERTY()
	TObjectPtr<UCadenceTriggerData> Value;
};