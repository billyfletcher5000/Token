// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceRhythmTrack.h"
#include "HarmonixMidi/MidiFile.h"
#include "CadenceRhythmTrackMidi.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceRhythmTrackMidi : public UCadenceRhythmTrack
{
	GENERATED_BODY()

public:
	UMidiFile* GetMidiFile() const { return MidiFile; }
	void SetMidiFile(UMidiFile* InMidiFile);

	virtual int32 GetLengthInNumberOfBoundaries(const EQuartzCommandQuantization InQuantizationBoundary) const override;
	
	virtual void Play() override;
	virtual void Stop() override;

protected:
	UPROPERTY()
	TObjectPtr<UMidiFile> MidiFile;
};
