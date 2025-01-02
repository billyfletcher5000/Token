// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cadence.h"

#include "SequencerTrack/CadenceSequenceActor.h"
#include "SequencerTrack/CadenceSequenceActorCustomization.h"

DEFINE_LOG_CATEGORY(LogCadence);
#define LOCTEXT_NAMESPACE "FCadenceModule"

void FCadenceModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(ACadenceSequenceActor::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic( &FCadenceSequenceActorCustomization::MakeInstance ) );
}

void FCadenceModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(ACadenceSequenceActor::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCadenceModule, Cadence)