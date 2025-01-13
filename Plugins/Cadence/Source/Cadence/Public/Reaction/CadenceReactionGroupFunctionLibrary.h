// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceReactionGroupFunctionLibrary.generated.h"

class UCadenceReactionGroup;
class UCadenceVariable;
/**
 * 
 */
UCLASS()
class UCadenceReactionGroupFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ReactionGroup", meta = (ReactionGroupPin="InReactionGroup"))
	static CADENCE_API UCadenceVariable* GetReactionGroupVariable(UObject* InInstigator, UCadenceReactionGroup* InReactionGroup, FName VariableName);
};
