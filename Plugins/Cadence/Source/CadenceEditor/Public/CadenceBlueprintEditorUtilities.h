// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CadenceBlueprintEditorUtilities.generated.h"

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceBlueprintEditorUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cadence Editor Utilities", meta = (WorldContext = WorldContextObject))
	static void CreateFrozenConstructionBlueprintCopy(AActor* InActor, UClass* InNewParentClass);
};
