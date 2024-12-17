// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceContext.h"
#include "UObject/Object.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.generated.h"

class UCadenceTriggerData;

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceVariable : public UObject
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const PURE_VIRTUAL(UCadenceVariable::GetPinSubCategory, return NAME_None;);
	virtual FName GetPinSubCategory() const { return NAME_None; }
	virtual UObject* GetPinSubCategoryObject() const { return nullptr; }
	virtual FLinearColor GetPinColor() const PURE_VIRTUAL(UCadenceVariable::GetPinSubCategory, return FLinearColor::White;);
	virtual FName GetDisplayName() const { return GetPinSubCategory(); }
	
	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) PURE_VIRTUAL();

	// Called when a variable's parent node, if it has one, is complete and has transferred data from this variable to others
	virtual void OnParentNodeReleased(UCadenceContext* InContext) {}

	virtual bool IsArray() const { return false; }
	virtual bool IsEnum() const  { return false; }
	virtual TSubclassOf<UCadenceVariable> GetArrayVariableType() const { return nullptr; }
	
	virtual bool SupportsDefault() const { return false; }
	virtual bool CanCreateUserVariableOfType() const { return true; }
	virtual void SetFromString(const FString& InStringValue) { }
	virtual FString ConvertToValueString() const { return FString(); }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUserVariableNameChanged, const FName& /* InName */);

	FOnUserVariableNameChanged OnUserVariableNameChanged;
	virtual void SetUserVariableName(const FName& InName)
	{
		if(UserVariableName != InName)
		{
			UserVariableName = InName;
			OnUserVariableNameChanged.Broadcast(UserVariableName);
		}
	}
	virtual FName GetUserVariableName() const { return UserVariableName; }

	virtual bool IsVisible() const { return bIsVisible; }
	virtual void SetVisible(const bool& InValue) { bIsVisible = InValue; }

	FText GetCategory() const { return Category; }
	void SetCategory(const FText& InCategory) { Category = InCategory; }
	
	FGuid GetGUID()
	{
		if(!GUID.IsValid())
		{
			GUID = FGuid::NewGuid();
		}

		return GUID;
	}

protected:
	UPROPERTY()
	FName UserVariableName = NAME_None;

	UPROPERTY()
	bool bIsVisible = true;

	UPROPERTY()
	FText Category;

private:
	UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid GUID;
};


UCLASS()
class CADENCE_API UCadenceVariableInt : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Integer; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Int; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableInt* CastedVariable = Cast<UCadenceVariableInt>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}

	int32 GetValue() const { return Value; }
	void SetValue(const int32& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	int32 Value;
};


UCLASS(EditInlineNew)
class CADENCE_API UCadenceVariableFloat : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Float; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Float; }
	
	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableFloat* CastedVariable = Cast<UCadenceVariableFloat>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	float GetValue() const { return Value; }
	void SetValue(const float& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	float Value;
};


UCLASS()
class CADENCE_API UCadenceVariableDouble : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Double; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Double; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableDouble* CastedVariable = Cast<UCadenceVariableDouble>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	double GetValue() const { return Value; }
	void SetValue(const double& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
	
private:
	UPROPERTY(EditAnywhere)
	double Value;
};

UCLASS()
class CADENCE_API UCadenceVariableBool : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Boolean; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Bool; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableBool* CastedVariable = Cast<UCadenceVariableBool>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	bool GetValue() const { return Value; }
	void SetValue(const bool& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	bool Value;
};

UCLASS()
class CADENCE_API UCadenceVariableVector : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Vector; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Vector; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableVector* CastedVariable = Cast<UCadenceVariableVector>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FVector GetValue() const { return Value; }
	void SetValue(const FVector& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif
	
private:
	UPROPERTY(EditAnywhere)
	FVector Value;
};

UCLASS()
class CADENCE_API UCadenceVariableVector2D : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Vector2; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Vector2; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableVector2D* CastedVariable = Cast<UCadenceVariableVector2D>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FVector2D GetValue() const { return Value; }
	void SetValue(const FVector2D& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	FVector2D Value;
};

UCLASS()
class CADENCE_API UCadenceVariableRotator : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Rotator; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Rotator; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableRotator* CastedVariable = Cast<UCadenceVariableRotator>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FRotator GetValue() const { return Value; }
	void SetValue(const FRotator& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	FRotator Value;
};

UCLASS()
class CADENCE_API UCadenceVariableString : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_String; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_String; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableString* CastedVariable = Cast<UCadenceVariableString>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FString GetValue() const { return Value; }
	void SetValue(const FString& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	FString Value;
};

UCLASS()
class CADENCE_API UCadenceVariableName : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Name; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Name; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableName* CastedVariable = Cast<UCadenceVariableName>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FName GetValue() const { return Value; }
	void SetValue(const FName& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	FName Value;
};

UCLASS()
class CADENCE_API UCadenceVariableText : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Text; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Text; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableText* CastedVariable = Cast<UCadenceVariableText>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	FText GetValue() const { return Value; }
	void SetValue(const FText& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	FText Value;
};

UCLASS()
class CADENCE_API UCadenceVariableUObject : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Object; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_UObject; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableUObject* CastedVariable = Cast<UCadenceVariableUObject>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	TObjectPtr<UObject> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UObject>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UObject> Value;
};


UCLASS()
class CADENCE_API UCadenceVariableUObjectArray : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Object; }
	virtual FName GetPinSubCategory() const override { return FCadencePinSubCategoryConstants::PSC_Array; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_UObject; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableUObjectArray* CastedVariable = Cast<UCadenceVariableUObjectArray>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}

	virtual bool IsArray() const override { return true; }	
	
	TArray<TObjectPtr<UObject>> GetValue() const { return Value; }
	void SetValue(const TArray<TObjectPtr<UObject>>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UObject>> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableEnum : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Enum; }
	virtual FName GetPinSubCategory() const override { return EnumType != nullptr ? FName(EnumType->GetName()) : NAME_None; }
	virtual UObject* GetPinSubCategoryObject() const override { return EnumType; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Enum; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableEnum* CastedVariable = Cast<UCadenceVariableEnum>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}

	virtual bool IsEnum() const override { return true; }
	
	int64 GetValue() const { return Value; }
	void SetValue(const int64& InValue) { Value = InValue; }

	template<typename T>
	T GetValue() const { return static_cast<T>(Value); }
	template<typename T>
	void SetValue(const T& InValue) { Value = static_cast<int64>(InValue);}

	UEnum* GetEnumType() const { return EnumType; }

	virtual bool SupportsDefault() const override { return true; }
	virtual bool CanCreateUserVariableOfType() const override { return EnumType != nullptr; }
	
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY()
	int64 Value;

protected:
	UPROPERTY()
	UEnum* EnumType = nullptr;
};

UCLASS()
class UCadenceVariableQuantizationPeriod : public UCadenceVariableEnum
{
	GENERATED_BODY()

public:
	UCadenceVariableQuantizationPeriod();
};

UCLASS()
class CADENCE_API UCadenceVariableActor : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Actor; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Actor; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override;

	virtual void OnParentNodeReleased(UCadenceContext* InContext) override;
	
	TObjectPtr<AActor> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<AActor>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableTrigger : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Trigger; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Trigger; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableTrigger* CastedVariable = Cast<UCadenceVariableTrigger>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	TObjectPtr<UCadenceTriggerData> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UCadenceTriggerData>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceTriggerData> Value;
};