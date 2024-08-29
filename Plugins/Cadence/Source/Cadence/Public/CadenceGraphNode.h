// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGraphNode.generated.h"

class UCadenceGraph;
class UCadenceGraphNodePin;

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceGraphNode : public UObject
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeTitle() const { return GetNodeMenuName(); }
	virtual FText GetNodeMenuName() const { return FText::FromString(StaticClass()->GetName()); }
	virtual FText GetNodeCategory() const { return FText::FromName(NAME_Default); }
	virtual FText GetCreateNodeTooltip() const { return FText::FromString(TEXT("Create new node"));}
	virtual FLinearColor GetNodeTitleColor() const { return FLinearColor::Blue; }
	
	virtual bool IsPure() const { return false; }
	virtual bool HasInputExecPin() const { return true; }
	virtual bool HasOutputExecPin() const { return true; }
	
	virtual void CreateInputPins();
	virtual void CreateOutputPins();

	TObjectPtr<UCadenceGraphNodePin> GetExecPin() const;
	TObjectPtr<UCadenceGraphNodePin> GetThenPin() const;

	virtual bool Execute() PURE_VIRTUAL(UCadenceGraphNode::Execute, return true;);
	
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() { return InputPins; }
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() { return OutputPins; }

	TObjectPtr<UCadenceGraphNodePin> GetInputPin(const FName& InPinName) const;
	TObjectPtr<UCadenceGraphNodePin> GetOutputPin(const FName& InPinName) const;

	void SetParentGraph(TObjectPtr<UCadenceGraph> InParentGraph) { ParentGraph = InParentGraph; }
	UCadenceGraph* GetParentGraph() const { return ParentGraph; }

	FVector2D GetPosition() const { return Position; }
	void SetPosition(const FVector2D& InPosition) { Position.X = InPosition.X; Position.Y = InPosition.Y; }
	void SetPosition(const double& X, const double& Y) { Position.X = X; Position.Y = Y; }
	
protected:
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputExecPin(const FName& InPinName);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputExecPin(const FName& InPinName);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual TObjectPtr<UCadenceGraphNodePin> CreateExecPin(const FName& InPinName);	
	virtual TObjectPtr<UCadenceGraphNodePin> CreateVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual bool RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);
	virtual bool RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);

private:
	static TObjectPtr<UCadenceGraphNodePin> GetPinFromArray(const TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName);
	
public:
	UPROPERTY()
	TObjectPtr<UCadenceGraph> ParentGraph;
	
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> OutputPins;

	UPROPERTY()
	FVector2D Position;
};
