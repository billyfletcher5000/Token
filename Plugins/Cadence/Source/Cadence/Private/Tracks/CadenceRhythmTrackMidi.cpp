// Fill out your copyright notice in the Description page of Project Settings.


#include "Tracks/CadenceRhythmTrackMidi.h"


void UCadenceRhythmTrackMidi::SetMidiFile(UMidiFile* InMidiFile)
{
	if (IsPlaying())
		Stop();
	
	MidiFile = InMidiFile;
}

void UCadenceRhythmTrackMidi::Play()
{
	Super::Play();
}

void UCadenceRhythmTrackMidi::Stop()
{
	Super::Stop();
}

int32 UCadenceRhythmTrackMidi::GetLengthInNumberOfBoundaries(const EQuartzCommandQuantization InQuantizationBoundary) const
{
	return Super::GetLengthInNumberOfBoundaries(InQuantizationBoundary);
}