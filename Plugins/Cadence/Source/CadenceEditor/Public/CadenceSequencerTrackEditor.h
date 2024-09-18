// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneTrackEditor.h"
#include "UObject/Object.h"

class UCadenceSequencerSection;
class UCadenceSequencerTrack;

class CADENCEEDITOR_API FCadenceSequencerTrackEditor : public FMovieSceneTrackEditor
{
public:
	FCadenceSequencerTrackEditor(TSharedRef<ISequencer> InSequencer);
	
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
	{
		return MakeShared<FCadenceSequencerTrackEditor>(InSequencer);
	}

protected:
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
	virtual const FSlateBrush* GetIconBrush() const override;

private:
	void AddNewTrack(FAssetData AssetData);
	void AddNewObjectBindingTrack(FAssetData AssetData, TArray<FGuid> InObjectBindings);
	void MakeMenuEntry(FMenuBuilder& MenuBuilder, UCadenceSequencerTrack* Track, TSubclassOf<UCadenceSequencerSection> ClassType);	
	void CreateNewSection(UCadenceSequencerTrack* Track, TSubclassOf<UCadenceSequencerSection> ClassType);
};
