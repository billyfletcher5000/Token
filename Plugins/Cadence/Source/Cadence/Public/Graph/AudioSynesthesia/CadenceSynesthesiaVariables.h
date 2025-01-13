// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoudnessNRT.h"
#include "ConstantQNRT.h"
#include "OnsetNRT.h"
#include "Graph/CadenceVariable.h"
#include "UObject/Object.h"
#include "CadenceSynesthesiaVariables.generated.h"

namespace FCadenceSynesthesiaConstants
{
	static const FName PC_LoudnessNRT = TEXT("Loudness NRT");
	static const FName PC_ConstantQNRT = TEXT("ConstantQ NRT");
	static const FName PC_OnsetNRT = TEXT("Onset NRT");

	static constexpr FLinearColor VC_NRT = FLinearColor(0.5f, 0.2f, 0.35f);	
}

UCLASS()
class CADENCE_API UCadenceVariableLoudnessNRT : public UCadenceVariable
{
	GENERATED_BODY()

public:	
	virtual FName GetPinCategory() const override {	return FCadenceSynesthesiaConstants::PC_LoudnessNRT; }
	virtual FLinearColor GetPinColor() const override { return FCadenceSynesthesiaConstants::VC_NRT; }
	virtual UObject* GetPinSubCategoryObject() const override { return ULoudnessNRT::StaticClass(); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableLoudnessNRT* CastedVariable = Cast<UCadenceVariableLoudnessNRT>(OtherVariable);
		if(ensure(CastedVariable))
			SetValue(CastedVariable->GetValue());
	}
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULoudnessNRT* GetValue() const { return Value; }
	void SetValue(const TObjectPtr<ULoudnessNRT>& InValue) { SetValueHelper(Value, InValue); }

	virtual bool SupportsDefault() const override { return true; }
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<ULoudnessNRT> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableConstantQNRT : public UCadenceVariable
{
	GENERATED_BODY()

public:	
	virtual FName GetPinCategory() const override {	return FCadenceSynesthesiaConstants::PC_ConstantQNRT; }
	virtual FLinearColor GetPinColor() const override { return FCadenceSynesthesiaConstants::VC_NRT; }
	virtual UObject* GetPinSubCategoryObject() const override { return UConstantQNRT::StaticClass(); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableConstantQNRT* CastedVariable = Cast<UCadenceVariableConstantQNRT>(OtherVariable);
		if(ensure(CastedVariable))
			SetValue(CastedVariable->GetValue());
	}
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UConstantQNRT* GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UConstantQNRT>& InValue) { SetValueHelper(Value, InValue); }

	virtual bool SupportsDefault() const override { return true; }
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UConstantQNRT> Value;
};

UCLASS()
class CADENCE_API UCadenceVariableOnsetNRT : public UCadenceVariable
{
	GENERATED_BODY()

public:	
	virtual FName GetPinCategory() const override {	return FCadenceSynesthesiaConstants::PC_ConstantQNRT; }
	virtual FLinearColor GetPinColor() const override { return FCadenceSynesthesiaConstants::VC_NRT; }
	virtual UObject* GetPinSubCategoryObject() const override { return UOnsetNRT::StaticClass(); }

	virtual void CopyValueFrom(UCadenceVariable* OtherVariable, UCadenceContext* InContext = nullptr) override
	{
		UCadenceVariableOnsetNRT* CastedVariable = Cast<UCadenceVariableOnsetNRT>(OtherVariable);
		if(ensure(CastedVariable))
			SetValue(CastedVariable->GetValue());
	}
	
	virtual bool Equals(UCadenceVariable* OtherVariable) override
	{
		return EqualsHelper(this, OtherVariable);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UOnsetNRT* GetValue() const { return Value; }
	void SetValue(const TObjectPtr<UOnsetNRT>& InValue) { SetValueHelper(Value, InValue); }

	virtual bool SupportsDefault() const override { return true; }
	virtual void SetFromString(const FString& InStringValue) override;
	virtual FString ConvertToValueString() const override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UOnsetNRT> Value;
};
