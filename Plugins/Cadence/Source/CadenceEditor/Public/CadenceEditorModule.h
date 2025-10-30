// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Utility/CadenceTransientDetector.h"

struct FCadenceTransientInfo;
class UMovieSceneAudioSection;
class UMovieSceneAudioTrack;
class ISequencer;
class UCadenceVariable;
class FCadenceGraphEditorNodeFactory;
class FCadenceGraphEditorPanelPinFactory;

DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<SWidget>, FVariableCreateInlineWidgetFunc, UCadenceVariable* /* InVariable */,
																					 TSharedPtr<IPropertyHandle> /* InPropertyHandle */);

typedef TMap<FName /* Class Name */, FVariableCreateInlineWidgetFunc> VariableInlineWidgetFuncMap;

class FCadenceEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void RegisterSequencerExtensions();
	void UnregisterSequencerExtensions();
	void MakeSelectTransientSettingsSubMenu(UToolMenu* ToolMenu);

public:
	static const FName CategoryKey;
	static const FText CategoryDisplayName;
	static const FName StyleSetName;

	VariableInlineWidgetFuncMap& GetMap() { return VariableToInlineWidgetFunc; }
	VariableInlineWidgetFuncMap VariableToInlineWidgetFunc;

	FSlateStyleSet* GetStyleSet() const { return StyleSet.Get(); }
	
private:
	void OnSequencerCreated(TSharedRef<ISequencer> InSequencer);
	void RegisterMenuExtensions();
	TSharedPtr<ISequencer> GetActiveSequencer();
	void ProcessAudioTrack(UMovieSceneAudioTrack* AudioTrack, TSharedPtr<ISequencer> Sequencer);	
	void ProcessAudioSection(UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer);
	void CreateMarkersFromTransients(const TArray<FCadenceTransientInfo>& Transients, UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer);

	TSharedPtr<FSlateStyleSet> StyleSet = nullptr;
	TSharedPtr<FCadenceGraphEditorPanelPinFactory> PanelPinFactory = nullptr;
	TSharedPtr<FCadenceGraphEditorNodeFactory> NodeFactory = nullptr;
	
	FDelegateHandle CustomTrackEditorHandle;
	// Handles for our extensions
	TArray<FDelegateHandle> SequencerExtenders;	
	TArray<TWeakPtr<ISequencer>> ActiveSequencers;
	TWeakObjectPtr<UCadenceTransientDetectorSettings> CurrentTransientDetectorSettings;
};
