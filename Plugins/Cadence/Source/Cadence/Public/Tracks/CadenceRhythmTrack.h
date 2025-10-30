// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICadenceTrack.h"
#include "UObject/Object.h"
#include "CadenceRhythmTrack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceRhythmTrack : public UObject, public ICadenceTrack
{
	GENERATED_BODY()

public:
	virtual void Play() override;
	virtual void Stop() override;
	virtual bool IsPlaying() const override { return bIsPlaying; }
	virtual bool AddChild(const TScriptInterface<ICadenceTrack>& InTrack) override;
	virtual bool RemoveChild(const TScriptInterface<ICadenceTrack>& InTrack) override;
	virtual TArray<TScriptInterface<ICadenceTrack>> GetChildren() const override { return Children; }

protected:
	UPROPERTY()
	bool bIsPlaying;
	
	UPROPERTY()
	TArray<TScriptInterface<ICadenceTrack>> Children;
};
