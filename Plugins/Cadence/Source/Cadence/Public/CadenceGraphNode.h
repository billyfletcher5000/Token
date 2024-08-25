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
	void ConnectPin(UCadenceGraphNodePin* InPin);
	void DisconnectPin(UCadenceGraphNodePin* InPin);

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
	const TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() const { return InputPins; }
	const TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() const { return OutputPins; }
	
	void AddInputPin(UCadenceGraphNodePin* InPin);
	void RemoveInputPin(UCadenceGraphNodePin* InPin);
	
	void AddOutputPin(UCadenceGraphNodePin* InPin);
	void RemoveOutputPin(UCadenceGraphNodePin* InPin);
	
protected:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> OutputPins;
};
