// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cadence.h"
#include "CadenceContext.h"
#include "Actors/CadenceActorLifetime.h"
#include "UObject/Object.h"
#include "CadenceGraphNode.h"
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
	
	virtual bool SupportsDefault() const { return false; }
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) { }
	virtual FString ConvertToValueString() const { return FString(); }
#endif

	virtual void SetUserVariableName(const FName& InName) { UserVariableName = InName; }
	virtual FName GetUserVariableName() const { return UserVariableName; }

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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.4f, 0.4f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableInt* IntVariable = Cast<UCadenceVariableInt>(OtherVariable);
		Value = IntVariable->GetValue();
	}

	int32 GetValue() const { return Value; }
	void SetValue(const int32& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.0f, 1.0f, 0.0f); }
	
	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableFloat* CastedVariable = Cast<UCadenceVariableFloat>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	float GetValue() const { return Value; }
	void SetValue(const float& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.4f, 0.4f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableDouble* CastedVariable = Cast<UCadenceVariableDouble>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	double GetValue() const { return Value; }
	void SetValue(const double& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif
	
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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(1.0f, 0.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableBool* CastedVariable = Cast<UCadenceVariableBool>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	bool GetValue() const { return Value; }
	void SetValue(const bool& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(1.0f, 1.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableVector* CastedVariable = Cast<UCadenceVariableVector>(OtherVariable);
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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.0f, 0.0f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableVector2D* CastedVariable = Cast<UCadenceVariableVector2D>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	FVector2D GetValue() const { return Value; }
	void SetValue(const FVector2D& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(1.0f, 0.7f, 0.2f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableRotator* CastedVariable = Cast<UCadenceVariableRotator>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	FRotator GetValue() const { return Value; }
	void SetValue(const FRotator& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

private:
	UPROPERTY(EditAnywhere)
	FRotator Value;
};

UCLASS()
class CADENCE_API UCadenceVariableUObject : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Object; }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableUObject* CastedVariable = Cast<UCadenceVariableUObject>(OtherVariable);
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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableUObjectArray* CastedVariable = Cast<UCadenceVariableUObjectArray>(OtherVariable);
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
class CADENCE_API UCadenceVariableQuartzCommandQuantization : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Enum; }
	virtual FName GetPinSubCategory() const override { return FName(StaticEnum<EQuartzCommandQuantization>()->GetName()); }
	virtual UObject* GetPinSubCategoryObject() const override { return StaticEnum<EQuartzCommandQuantization>(); }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 1.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableQuartzCommandQuantization* CastedVariable = Cast<UCadenceVariableQuartzCommandQuantization>(OtherVariable);
		Value = CastedVariable->GetValue();
	}

	virtual bool IsEnum() const override { return true; }
	
	EQuartzCommandQuantization GetValue() const { return Value; }
	void SetValue(const EQuartzCommandQuantization& InValue) { Value = InValue; }

	virtual bool SupportsDefault() const override { return true; }
	
#if WITH_EDITOR
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;
#endif

private:
	UPROPERTY()
	EQuartzCommandQuantization Value;
};

UCLASS()
class CADENCE_API UCadenceVariableActor : public UCadenceVariable
{
	GENERATED_BODY()

public:
	virtual FName GetPinCategory() const override { return FCadencePinCategoryConstants::PC_Actor; }
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.3f, 0.3f, 0.95f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
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

	virtual void OnParentNodeReleased(UCadenceContext* InContext) override
	{
		if(ensure(InContext))
		{
			FGuid TempGUID = GetGUID();
			UE_LOG(LogCadence, Log, TEXT("OnParentNodeReleased Unregister: %s - %s"), *GetName(), *TempGUID.ToString());
			InContext->ActorLifetimeManager->UnregisterActorUsage(Value, TempGUID);
		}
	}
	
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
	virtual FLinearColor GetPinColor() const override { return FLinearColor(0.75f, 0.0f, 0.0f); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableTrigger* CastedVariable = Cast<UCadenceVariableTrigger>(OtherVariable);
		Value = CastedVariable->GetValue();
	}
	
	TObjectPtr<UCadenceTriggerData> GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UCadenceTriggerData>& InValue) { Value = InValue; }

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceTriggerData> Value;
};