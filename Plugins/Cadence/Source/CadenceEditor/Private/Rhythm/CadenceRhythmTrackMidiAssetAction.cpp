// Fill out your copyright notice in the Description page of Project Settings.


#include "Rhythm/CadenceRhythmTrackMidiAssetAction.h"

#include "Tracks/CadenceRhythmTrackMidi.h"


const FText FCadenceRhythmTrackMidiAssetAction::AssetName = FText::FromString(TEXT("Cadence Rhythm"));
const FColor FCadenceRhythmTrackMidiAssetAction::TypeColor = FColor::Purple;

FCadenceRhythmTrackMidiAssetAction::FCadenceRhythmTrackMidiAssetAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText FCadenceRhythmTrackMidiAssetAction::GetName() const
{
	return AssetName;
}

FColor FCadenceRhythmTrackMidiAssetAction::GetTypeColor() const
{
	return TypeColor;
}

UClass* FCadenceRhythmTrackMidiAssetAction::GetSupportedClass() const
{
	return UCadenceRhythmTrackMidi::StaticClass();
}

void FCadenceRhythmTrackMidiAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

uint32 FCadenceRhythmTrackMidiAssetAction::GetCategories()
{
	return AssetCategory;
}
