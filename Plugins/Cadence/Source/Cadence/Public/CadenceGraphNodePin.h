// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceGraphNodePin.generated.h"

class UCadenceVariable;

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceGraphNodePin : public UObject
{
	GENERATED_BODY()
	
public:
	const TArray<TObjectPtr<UCadenceGraphNodePin>>& GetConnectedPins() const { return ConnectedPins; }
	void SetConnectedPins(TArray<TObjectPtr<UCadenceGraphNodePin>> InPins);
	
	void ConnectPin(UCadenceGraphNodePin* InPin);	
	void DisconnectPin(UCadenceGraphNodePin* InPin);

	void SetPinName(const FName& InPinName) { PinName = InPinName; }
	FName GetPinName() const { return PinName; }
	
	void GenerateGUID();
	void SetGUID(const FGuid& InGUID) { GUID = InGUID; }
	FGuid GetGUID() const { return GUID; }

	TSubclassOf<UCadenceVariable> GetVariableClass() const { return VariableClass; }
	void SetVariableClass(TSubclassOf<UCadenceVariable> InVariableClass) { VariableClass = InVariableClass; }
	
	TSharedPtr<UCadenceVariable> GetVariable() const { return Variable.Pin(); }
	void SetVariable(const TSharedPtr<UCadenceVariable>& InVariable) { Variable = InVariable; }
	
private:
	UPROPERTY()
	FName PinName;
	
	UPROPERTY()
	FGuid GUID;
	
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins;

	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableClass;

private:
	TWeakPtr<UCadenceVariable> Variable;
};
