// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceAsset.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "CadenceSequenceActor.generated.h"

class UMovieSceneSequencePlayer;

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
	
	void OnPlayerUpdated(const UMovieSceneSequencePlayer& Player, FFrameTime CurrentTime, FFrameTime PreviousTime);
};
