// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CadenceBlueprintUtilities.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceBlueprintUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ActorFuncions", meta = (WorldContext = WorldContextObject))
	static AActor* DuplicateActorComponentCopy(AActor* InActor, UClass* InNewActorClass);
};
