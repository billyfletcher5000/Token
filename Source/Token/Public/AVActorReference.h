// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"

#include "AVActorReference.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TOKEN_API UAVActorReferenceDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	FGuid GUID;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	FName SceneName;
};