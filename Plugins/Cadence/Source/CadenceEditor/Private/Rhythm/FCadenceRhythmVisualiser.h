// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISequencer.h"
#include "Utility/CadenceTransientDetector.h"

struct FCadenceTransientInfo;
class UMovieSceneAudioSection;
class UMovieSceneAudioTrack;
/**
 * 
 */
class CADENCEEDITOR_API FCadenceRhythmVisualiser
{
public:
	void OnStartupModule();
	void OnShutdownModule();

private:
	void RegisterSequencerExtensions();
	void UnregisterSequencerExtensions();
	void MakeSelectTransientSettingsSubMenu(UToolMenu* ToolMenu);
	
	void OnSequencerCreated(TSharedRef<ISequencer> InSequencer);
	void RegisterMenuExtensions();
	TSharedPtr<ISequencer> GetActiveSequencer();
	void ProcessAudioTrack(UMovieSceneAudioTrack* AudioTrack, TSharedPtr<ISequencer> Sequencer);	
	void ProcessAudioSection(UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer);
	void CreateMarkersFromTransients(const TArray<FCadenceTransientInfo>& Transients, UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer);

	
	// Handles for our extensions
	TArray<FDelegateHandle> SequencerExtenders;	
	TArray<TWeakPtr<ISequencer>> ActiveSequencers;
	TWeakObjectPtr<UCadenceTransientDetectorSettings> CurrentTransientDetectorSettings;
};
