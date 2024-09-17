// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneNameableTrack.h"
#include "UObject/Object.h"
#include "CadenceSequencerTrack.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CADENCE_API UCadenceSequencerTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Sequencer", EditDefaultsOnly, AssetRegistrySearchable)
	FSlateBrush Icon;

public:

	virtual bool SupportsMultipleRows() const override { return true; }
	virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const override { return EMovieSceneTrackEasingSupportFlags::None; }
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual UMovieSceneSection* CreateNewSection() override;

	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override { return Sections; }
	virtual void AddSection(UMovieSceneSection& Section) override { Sections.AddUnique(&Section); }
	virtual void RemoveSection(UMovieSceneSection& Section) override { Sections.Remove(&Section); }
	virtual void RemoveSectionAt(int32 SectionIndex) override { Sections.RemoveAt(SectionIndex); }
	virtual bool HasSection(const UMovieSceneSection& Section) const override { return Sections.Contains(&Section); }
	virtual bool IsEmpty() const override { return Sections.Num() == 0; }
	
private:

	UPROPERTY(Instanced)
	TArray<TObjectPtr<UMovieSceneSection>> Sections;
};
