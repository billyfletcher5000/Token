// Fill out your copyright notice in the Description page of Project Settings.

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
