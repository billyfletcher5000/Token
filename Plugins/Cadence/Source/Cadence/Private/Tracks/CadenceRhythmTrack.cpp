// Fill out your copyright notice in the Description page of Project Settings.


#include "Tracks/CadenceRhythmTrack.h"

void UCadenceRhythmTrack::Play()
{
	bIsPlaying = true;
}

void UCadenceRhythmTrack::Stop()
{
	bIsPlaying = false;
	
	for (auto& ChildTrack : Children)
	{
		ChildTrack->Stop();
	}
}

bool UCadenceRhythmTrack::AddChild(const TScriptInterface<ICadenceTrack>& InTrack)
{
	if (!ensureAlways(InTrack))
		return false;

	if (Children.Contains(InTrack))
		return false;
	
	Children.Add(InTrack);
	return true;
}

bool UCadenceRhythmTrack::RemoveChild(const TScriptInterface<ICadenceTrack>& InTrack)
{
	if (!ensureAlways(InTrack))
		return false;

	return Children.Remove(InTrack) > 0;
}
