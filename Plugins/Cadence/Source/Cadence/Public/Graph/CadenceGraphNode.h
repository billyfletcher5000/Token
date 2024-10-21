// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGraphNode.generated.h"

class UCadenceGraph;
class UCadenceGraphNodePin;
class UCadenceContext;

UENUM()
enum class ECadenceNodeExecuteResult : uint8
{
	Running = 0,
	Complete,
	Failed
};

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceGraphNode : public UObject
{
	GENERATED_BODY()
	
public:
	void GenerateGUID();
	
	virtual FText GetNodeTitle() const { return GetNodeMenuName(); }
	virtual FText GetNodeMenuName() const { return FText::FromString(StaticClass()->GetName()); }
	virtual FText GetNodeCategory() const { return FText::FromName(NAME_Default); }
	virtual FText GetCreateNodeTooltip() const { return FText::FromString(TEXT("Create new node"));}
	virtual FLinearColor GetNodeTitleColor() const { return FLinearColor::Blue; }
	
	virtual bool IsPure() const { return false; }
	virtual bool IsReroute() const { return false; }
	virtual bool IsLatent() const { return false; }
	
	virtual bool HasInputExecPin() const { return true; }
	virtual bool HasOutputExecPin() const { return true; }

	virtual void DestroyNode();
	
	virtual void CreateInputPins();
	virtual void CreateOutputPins();
	virtual void ClearConnections();

	TObjectPtr<UCadenceGraphNodePin> GetExecPin() const;
	TObjectPtr<UCadenceGraphNodePin> GetThenPin() const;

	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) PURE_VIRTUAL(UCadenceGraphNode::Execute, return ECadenceNodeExecuteResult::Complete;);
	
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() { return InputPins; }
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() { return OutputPins; }

	TObjectPtr<UCadenceGraphNodePin> GetInputPin(const FName& InPinName) const;
	TObjectPtr<UCadenceGraphNodePin> GetOutputPin(const FName& InPinName) const;

	UCadenceGraphNodePin* GetMostAppropriateAutomaticInputPin(UCadenceGraphNodePin* OtherPin);
	UCadenceGraphNodePin* GetMostAppropriateAutomaticOutputPin(UCadenceGraphNodePin* OtherPin);
	
	void SetParentGraph(TObjectPtr<UCadenceGraph> InParentGraph) { ParentGraph = InParentGraph; }
	UCadenceGraph* GetParentGraph() const { return ParentGraph; }

	FVector2D GetGraphPosition() const { return GraphPosition; }
	void SetGraphPosition(const FVector2D& InGraphPosition) { GraphPosition.X = InGraphPosition.X; GraphPosition.Y = InGraphPosition.Y; }
	void SetGraphPosition(const double& X, const double& Y) { GraphPosition.X = X; GraphPosition.Y = Y; }

	FGuid GetGUID() const { return GUID; }
	FString GetDebugName() const { return DebugName; }	
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const { return true; }
	virtual bool DoesSupportEditorValuePropagation() const { return false; }
#endif
	
protected:
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputExecPin(const FName& InPinName);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputExecPin(const FName& InPinName);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const bool& bInOptional = false);
	virtual TObjectPtr<UCadenceGraphNodePin> CreateExecPin(const FName& InPinName);	
	virtual TObjectPtr<UCadenceGraphNodePin> CreateVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass);
	virtual bool RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);
	virtual bool RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);
	virtual void RemoveAllInputPins();
	virtual void RemoveAllOutputPins();
	virtual void RemoveAllPins();

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
	FVector2D GraphPosition;

private:
	UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid GUID;

	UPROPERTY(EditAnywhere)
	FString DebugName;
};
