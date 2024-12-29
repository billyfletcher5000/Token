// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceContext.h"
#include "UObject/Object.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.generated.h"

class UCadenceTriggerData;

/**
 * NOTE: If adding a new Variable derivative, you must have your main value property named "Value" or the user variable details view will not be able
 *		 to appropriately display it within arrays of that type
 */
UCLASS(Abstract, EditInlineNew)
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
	virtual bool Equals(UCadenceVariable* OtherVariable) PURE_VIRTUAL(UCadenceVariable::Equals, return false;);

	// Called when a variable's parent node, if it has one, is complete and has transferred data from this variable to others
	virtual void OnParentNodeReleased(UCadenceContext* InContext) {}

	virtual bool IsArray() const { return false; }
	virtual bool IsEnum() const  { return false; }
	
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

	virtual bool IsPublic() const { return bIsPublic; }
	virtual void SetIsPublic(const bool& InValue) { bIsPublic = InValue; }

	virtual bool IsOutput() const { return bIsOutput; }
	virtual void SetIsOutput(const bool& InValue) { bIsOutput = InValue; }

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
	template<typename T>
	static bool EqualsHelper(T* ThisVariable, UCadenceVariable* OtherVariable)
	{
		T* CastedVariable = Cast<T>(OtherVariable);
		if(ensure(CastedVariable))
			return CastedVariable->GetValue() == ThisVariable->GetValue();

		return false;
	}
	
	UPROPERTY()
	FName UserVariableName = NAME_None;

	UPROPERTY(EditAnywhere)
	bool bIsPublic = true;

	UPROPERTY(EditAnywhere)
	bool bIsOutput = false;

	UPROPERTY()
	FText Category;

private:
	UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid GUID;
};

UCLASS()
class CADENCE_API UCadenceVariableArray : public UCadenceVariable
{
	GENERATED_BODY()
	
public:
	static UCadenceVariableArray* Create(TSubclassOf<UCadenceVariable> InVariableClass, UObject* InOuter);
	
	virtual FName GetPinCategory() const override;
	virtual FName GetPinSubCategory() const override { return FCadencePinSubCategoryConstants::PSC_Array; }
	virtual FLinearColor GetPinColor() const override;

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override;
	virtual bool Equals(UCadenceVariable* OtherVariable) override;
	
	virtual bool IsArray() const override { return true; }	

	TArray<UCadenceVariable*> GetValue() const { return Value; }
	void SetValue(const TArray<UCadenceVariable*>& InValue);

	TSubclassOf<UCadenceVariable> GetVariableClass() const { return VariableClass; }
	void SetVariableClass(const TSubclassOf<UCadenceVariable>& InVariableClass);

	int32 GetSize() const { return Value.Num(); }
	UCadenceVariable* GetElement(const int32& InElementIndex) const;
	int32 GetIndexOfElement(UCadenceVariable* InVariable) const;
	bool ContainsElement(UCadenceVariable* InVariable) const;
	int32 AddElement(UCadenceVariable* InVariable);
	int32 RemoveElement(UCadenceVariable* InVariable);
	bool RemoveElement(const int32& InElementIndex);
	void EmptyElements();
	
private:
	UPROPERTY(EditAnywhere)
	TArray<UCadenceVariable*> Value;

	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableClass;
};

UCLASS()
class CADENCE_API UCadenceVariableInt : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override {	return FCadencePinCategoryConstants::PC_Integer; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_Int; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableInt* CastedVariable = Cast<UCadenceVariableInt>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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

UCLASS(EditInlineNew)
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override;
	
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	TObjectPtr<UObject> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UObject>& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UObject> Value;
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}

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
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	TObjectPtr<UCadenceTriggerData> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UCadenceTriggerData>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceTriggerData> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableCadenceAsset : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_CadenceAsset; }
	virtual FLinearColor GetPinColor() const override { return FCadenceVariableColorConstants::VC_CadenceAsset; }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableCadenceAsset* CastedVariable = Cast<UCadenceVariableCadenceAsset>(OtherVariable);
		if(ensure(CastedVariable))
			Value = CastedVariable->GetValue();
	}
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	TObjectPtr<UCadenceAsset> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UCadenceAsset>& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceAsset> Value;
};