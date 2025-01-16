// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class UCadenceVariable;
class UCadenceOperation;

class FCadenceGraphOperationResolver
{
public:
	FCadenceGraphOperationResolver();

	void UpdateClassList();

	UCadenceOperation* TryCreateMostAppropriateOperation(const TSubclassOf<UCadenceOperation>& InOperationBase, const TSubclassOf<UCadenceVariable>& InVariableClassA, const TSubclassOf<UCadenceVariable>& InVariableClassB, UObject* InOuter, const bool& InAreMultipleOfA = false, const bool& InAreMultipleOfB = false);

private:
	TArray<TSubclassOf<UCadenceOperation>> OperationTypes;
};
