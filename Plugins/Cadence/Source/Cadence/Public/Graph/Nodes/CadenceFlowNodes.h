// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceLatentNode.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNode.h"
#include "TickableActions/ICadenceTickableAction.h"
#include "UObject/Object.h"
#include "CadenceFlowNodes.generated.h"

namespace FCadenceFlowControlConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Flow Control"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.5f, 0.5f);
}

UCLASS()
class CADENCE_API UCadenceBranchNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Branch")); }
	virtual FText GetNodeCategory() const override { return FCadenceFlowControlConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceFlowControlConstants::NodeTitleColor; }

	virtual bool ShouldCreateThenExecPin() const override { return false; }
	virtual TArray<UCadenceGraphNodePin*> GetActuatingOutputExecPins() const override;
};

UCLASS()
class CADENCE_API UCadenceForNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("For")); }
	virtual FText GetNodeCategory() const override { return FCadenceFlowControlConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceFlowControlConstants::NodeTitleColor; }

	virtual bool ShouldCreateThenExecPin() const override { return false; }
	virtual TArray<UCadenceGraphNodePin*> GetActuatingOutputExecPins() const override;
};

UCLASS()
class CADENCE_API UCadenceForLoopRunner : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()

public:
	static UCadenceForLoopRunner* Create(const int32& InFirstIndex, const int32& InLastIndex,
		UCadenceGraphRunner* InGraphRunner, UCadenceGraphNode* InParentNode, UCadenceGraphNodePin* InIndexPin,
		const bool& bInExecuteImmediately = false, const float& InDeltaSeconds = 0.0f);
	
	virtual void Init() override;
	virtual bool Tick(const float& InDeltaSeconds) override;

protected:
	void OnPathwayEnded();

	void CreateNextPathway();
	
protected:
	TWeakObjectPtr<UCadenceGraphRunner> Runner;
	TWeakObjectPtr<UCadenceGraphNode> ParentNode;
	int32 FirstIndex = 0;
	int32 LastIndex = 0;
	int32 CurrentIndex = 0;
	TWeakObjectPtr<UCadenceGraphNodePin> IndexPin;
	bool bExecuteImmediately = false;
	float LastDeltaSeconds = 0.0f;
	bool bHasOutstandingPathway = false;
};

UCLASS(Abstract)
class CADENCE_API UCadenceRunGraphNode_Base : public UCadenceLatentNode
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() override;
	virtual UCadenceGraph* GetGraph() const PURE_VIRTUAL(UCadenceGraph::GetGraph, return nullptr;);
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const override { return false; }
#endif

protected:
	void UpdateVariablePins();
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;

	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> VariableInputPins;

	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> VariableOutputPins;
};

UCLASS()
class UCadenceRunGraphTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceRunGraphTickable* Create(UCadenceContext* InContext, UCadenceGraph* InTargetGraph, const TArray<UCadenceVariable*>& InInputVariables, UObject* InOuter);
	virtual void Init() override;
	virtual bool Tick(const float& InDeltaSeconds) override;

private:
	TWeakObjectPtr<UCadenceContext> Context;
	TWeakObjectPtr<UCadenceGraph> TargetGraph;
	TArray<TWeakObjectPtr<UCadenceVariable>> InputVariables;
	TWeakObjectPtr<UCadenceGraphRunner> Runner; 
};

UCLASS()
class CADENCE_API UCadenceRunGraphAssetNode : public UCadenceRunGraphNode_Base
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() override;
	virtual UCadenceGraph* GetGraph() const;
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const override { return true; }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Play Graph (Asset)")); }
	virtual FText GetNodeCategory() const override { return FCadenceFlowControlConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceFlowControlConstants::NodeTitleColor; }

private:
	TWeakObjectPtr<UCadenceVariable> ValueChangedVariable;
	FDelegateHandle OnGraphPinValueChangedHandle;
};