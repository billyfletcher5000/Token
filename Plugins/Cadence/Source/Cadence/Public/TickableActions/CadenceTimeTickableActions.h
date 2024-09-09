// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICadenceTickableAction.h"

#include "CadenceTimeTickableActions.generated.h"

UCLASS()
class UCadenceDelayTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceDelayTickable* Create(const float& InDuration)
	{
		UCadenceDelayTickable* Action = NewObject<UCadenceDelayTickable>();
		Action->Duration = InDuration;
		return Action;		
	}

	virtual bool Tick(const float& InDeltaSeconds) override
	{
		TimeElapsed += InDeltaSeconds;
		return TimeElapsed > Duration;
	}

private:
	float Duration = 10.0f;
	float TimeElapsed = 0.0f;
};