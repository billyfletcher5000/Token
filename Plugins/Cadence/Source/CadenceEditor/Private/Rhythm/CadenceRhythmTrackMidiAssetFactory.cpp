// Fill out your copyright notice in the Description page of Project Settings.


#include "Rhythm/CadenceRhythmTrackMidiAssetFactory.h"

#include "Tracks/CadenceRhythmTrackMidi.h"

UCadenceRhythmTrackMidiAssetFactory::UCadenceRhythmTrackMidiAssetFactory(const FObjectInitializer& ObjectInitializer)
{
	SupportedClass = UCadenceRhythmTrackMidi::StaticClass();
}

UObject* UCadenceRhythmTrackMidiAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCadenceRhythmTrackMidi>(InParent, InName, Flags);	
}

bool UCadenceRhythmTrackMidiAssetFactory::CanCreateNew() const
{
	return true;
}
