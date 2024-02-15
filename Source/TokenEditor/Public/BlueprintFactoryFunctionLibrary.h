// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintFactoryFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TOKENEDITOR_API UBlueprintFactoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static UFactory* CreateBlueprintFactory(const TSubclassOf<UObject> BPClass);

	UFUNCTION(BlueprintPure)
	static UFactory* CreateDataAssetFactory(const TSubclassOf<UDataAsset> DataAssetClass);
};
