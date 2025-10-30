// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CadenceActorSettings.generated.h"

class ACadenceMeshSplineActor;
/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceActorSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<ACadenceMeshSplineActor> GetLineMeshSplineActorType() const { return LineMeshSplineActor; }

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACadenceMeshSplineActor> LineMeshSplineActor;	
};
