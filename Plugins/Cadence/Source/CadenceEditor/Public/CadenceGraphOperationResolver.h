// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class UCadenceVariable;
class UCadenceOperation;

struct FCadenceOperationBaseBucket
{
	TSubclassOf<UCadenceOperation> Class;
	TArray<FCadenceOperationBaseBucket> ChildBuckets;
	TArray<TSubclassOf<UCadenceOperation>> LeafClasses;
};

struct FCadenceOperationResolverResult
{
	TSubclassOf<UCadenceOperation> OperationType;
	TSubclassOf<UCadenceVariable> PrimaryVariableType;
	TSubclassOf<UCadenceVariable> SecondaryVariableType;
	TSubclassOf<UCadenceVariable> ResultVariableType;
	bool bIsReversedOrder = false;
};

class FCadenceGraphOperationResolver
{
public:
	FCadenceGraphOperationResolver();

	void UpdateClassList();

	TArray<FCadenceOperationResolverResult> GetAppropriateOperationClasses(const TSubclassOf<UCadenceOperation>& InOperationBase,
														 const TSubclassOf<UCadenceVariable>& InVariableClassA,
														 const TSubclassOf<UCadenceVariable>& InVariableClassB,														 
														 const TSubclassOf<UCadenceVariable>& InVariableClassReturn,
														 const bool& InAreMultipleOfA,
														 const bool& InAreMultipleOfB);

private:
	TArray<FCadenceOperationResolverResult> GetAppropriateOperationClassesInternal(const TSubclassOf<UCadenceOperation>& InOperationBase,
														 const TSubclassOf<UCadenceVariable>& InVariableClassA,
														 const TSubclassOf<UCadenceVariable>& InVariableClassB,														 
														 const TSubclassOf<UCadenceVariable>& InVariableClassResult,														 
														 const bool& InAreMultipleOfB, const bool& bIsReverseOrder);

	
	FCadenceOperationBaseBucket RootBucket;
};
