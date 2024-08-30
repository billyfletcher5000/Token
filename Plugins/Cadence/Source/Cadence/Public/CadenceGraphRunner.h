// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceContext.h"
#include "UObject/Object.h"
#include "CadenceGraphRunner.generated.h"

class UCadenceGraphNodePin;
class UCadenceGraphNode;
class UCadenceContext;
class UCadenceGraph;
class UCadenceGraphRunnerPathway;

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceGraphRunner : public UObject
{
	GENERATED_BODY()

public:
	void Init(UCadenceContext* InContext);
	void Begin();
	void Tick(const float& InDeltaSeconds);
	void End();

protected:
	friend UCadenceGraphRunnerPathway;

	void NotifyPathwayEnded(UCadenceGraphRunnerPathway* InPathway);
	void RequestAdditionalPathway(UCadenceGraphNode* InStartNode);

private:
	UCadenceContext* GetContext();
	
private:
	UPROPERTY()
	UCadenceContext* Context;

	UPROPERTY()
	TArray<UCadenceGraphRunnerPathway*> ActivePathways;

	UPROPERTY()
	TArray<UCadenceGraphRunnerPathway*> EndedPathways;
};

UCLASS()
class CADENCE_API UCadenceGraphRunnerPathway : public UObject
{
	GENERATED_BODY()

public:
	void Init(UCadenceContext* InContext, UCadenceGraphNode* InStartNode);
	void Tick(const float& InDeltaSeconds);
	void End();

private:
	UCadenceContext* GetContext();

	bool ExecuteNode(UCadenceGraphNode* InNode, UCadenceContext* InContext);
	void ProcessVariableInputPin(UCadenceContext* InContext, UCadenceGraphNodePin* InPin);
	void GatherPureNodesContributingToPin(UCadenceGraphNodePin* InPin, TArray<UCadenceGraphNode*>& InNodeStack);

private:
	UPROPERTY()
	UCadenceContext* Context;

	UPROPERTY()
	UCadenceGraphNode* CurrentNode;
};