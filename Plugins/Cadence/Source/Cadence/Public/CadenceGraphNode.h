// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGraphNode.generated.h"

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

public:
	UPROPERTY()
	FLinearColor PinColor;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins;	
};


/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceGraphNode : public UObject
{
	GENERATED_BODY()
	
public:
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() { return InputPins; }
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() { return OutputPins; }
	
protected:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> OutputPins;
};
