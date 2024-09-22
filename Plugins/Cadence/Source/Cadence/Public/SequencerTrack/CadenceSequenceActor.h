// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceAsset.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "CadenceSequenceActor.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API ACadenceSequenceActor : public ALevelSequenceActor
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceAsset> CadenceGraph;

private:
	UFUNCTION()
	void OnPlayerStarted();

	UFUNCTION()
	void OnPlayerStopped();
};
