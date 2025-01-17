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

class FCadenceGraphOperationResolver
{
public:
	FCadenceGraphOperationResolver();

	void UpdateClassList();

	UCadenceOperation* TryCreateMostAppropriateOperation(const TSubclassOf<UCadenceOperation>& InOperationBase,
														 const TSubclassOf<UCadenceVariable>& InVariableClassA,
														 const TSubclassOf<UCadenceVariable>& InVariableClassB,														 
														 const TSubclassOf<UCadenceVariable>& InVariableClassReturn,
														 UObject* InOuter,
														 bool& OutPrimarySecondaryReversed,
														 const bool& InAreMultipleOfA = false,
														 const bool& InAreMultipleOfB = false);

private:
	UCadenceOperation* TryCreateMostAppropriateOperation(const TSubclassOf<UCadenceOperation>& InOperationBase,
														 const TSubclassOf<UCadenceVariable>& InVariableClassA,
														 const TSubclassOf<UCadenceVariable>& InVariableClassB,														 
														 const TSubclassOf<UCadenceVariable>& InVariableClassResult,
														 UObject* InOuter,
														 const bool& InAreMultipleOfB = false);

	
	FCadenceOperationBaseBucket RootBucket;
};
