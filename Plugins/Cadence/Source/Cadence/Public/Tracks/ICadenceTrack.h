// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICadenceTrack.generated.h"

UINTERFACE()
class UCadenceTrack : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CADENCE_API ICadenceTrack
{
	GENERATED_BODY()
	
public:
	virtual void Play() PURE_VIRTUAL();
	virtual void Stop() PURE_VIRTUAL();
	virtual bool IsPlaying() const PURE_VIRTUAL(ICadenceTrack::IsPlaying, return false;);
	virtual int32 GetLengthInNumberOfBoundaries(const EQuartzCommandQuantization InQuantizationBoundary) const PURE_VIRTUAL(ICadenceTrack::GetLengthInNumberOfBoundaries, return 0; );
	virtual bool AddChild(const TScriptInterface<ICadenceTrack>& InTrack) PURE_VIRTUAL(ICadenceTrack::AddChild, return false;);
	virtual bool RemoveChild(const TScriptInterface<ICadenceTrack>& InTrack) PURE_VIRTUAL(ICadenceTrack::RemoveChild, return false;);
	virtual TArray<TScriptInterface<ICadenceTrack>> GetChildren() const PURE_VIRTUAL(ICadenceTrack::GetChildren, return TArray<TScriptInterface<ICadenceTrack>>(););
};
