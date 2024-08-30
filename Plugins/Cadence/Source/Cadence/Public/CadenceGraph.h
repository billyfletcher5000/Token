// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceGraph.generated.h"

class UCadenceGraphNode;
/**
 * 
 */
UCLASS(BlueprintType)
class CADENCE_API UCadenceGraph : public UObject
{
	GENERATED_BODY()

public:
	TObjectPtr<UCadenceGraphNode> CreateNode(TSubclassOf<UCadenceGraphNode> InClass, const FVector2D& Location);
	
	void AddNode(TObjectPtr<UCadenceGraphNode> InNode);
	void RemoveNode(TObjectPtr<UCadenceGraphNode> InNode);
	void ClearNodes();

	TArray<TObjectPtr<UCadenceGraphNode>> GetNodes() const { return Nodes; }
	TArray<TObjectPtr<UCadenceGraphNode>> GetRootExecNodes() const;

public:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNode>> Nodes;
};
