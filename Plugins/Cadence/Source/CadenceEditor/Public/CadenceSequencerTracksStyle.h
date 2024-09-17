// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class UCadenceSequencerTrack;

class FCadenceSequencerTracksStyle : public FSlateStyleSet
{
public:
	static FName StyleName;

	/** Access the singleton instance for this style set */
	static FCadenceSequencerTracksStyle& Get();

	void RegisterNewTrackType(TSubclassOf<UCadenceSequencerTrack> TrackType);

private:
	FCadenceSequencerTracksStyle();
	~FCadenceSequencerTracksStyle();
};
