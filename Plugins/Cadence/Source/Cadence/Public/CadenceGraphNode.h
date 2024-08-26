// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGraphNode.generated.h"

class UCadenceGraphNodePin;

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceGraphNode : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() {}
	virtual void CreateOutputPins() {}

	virtual bool Execute() PURE_VIRTUAL(UCadenceGraphNode::Execute, return true;);
	
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() { return InputPins; }
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() { return OutputPins; }

	TObjectPtr<UCadenceGraphNodePin> GetInputPin(const FName& InPinName);
	TObjectPtr<UCadenceGraphNodePin> GetOutputPin(const FName& InPinName);

protected:
	virtual void AddInputPin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual void AddOutputPin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual TObjectPtr<UCadenceGraphNodePin> CreatePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);	

private:
	static TObjectPtr<UCadenceGraphNodePin> GetPin(TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName);
	
protected:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> OutputPins;
};
