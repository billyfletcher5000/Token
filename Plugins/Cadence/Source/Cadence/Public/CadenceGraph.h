// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceGraph.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CADENCE_API UCadenceGraph : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString ArbitraryData = TEXT("Blongobongo!");

	UPROPERTY(EditAnywhere)
	int32 ArbitraryNumber = 5;

	UPROPERTY(EditAnywhere)
	bool ArbitraryBool = false;
};
