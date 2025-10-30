// Copyright (C) Billy Fletcher 2025


#include "SequencerTrack/CadenceSequencerTrack.h"

#include "SequencerTrack/CadenceSequencerSection.h"

UCadenceSequencerTrack::UCadenceSequencerTrack()
{
	TrackName = FText::FromString("Cadence Track");
}

bool UCadenceSequencerTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	return SectionClass->IsChildOf(UCadenceSequencerSection::StaticClass());
}

UMovieSceneSection* UCadenceSequencerTrack::CreateNewSection()
{	
	return NewObject<UCadenceSequencerSection>(this, NAME_None, RF_Transactional);
}

bool UCadenceSequencerTrack::HasMultipleSectionsWithName(const FString& InName) const
{
	int32 Count = 0;

	for(UMovieSceneSection* MovieSceneSection : GetAllSections())
	{
		UCadenceSequencerSection* Section = CastChecked<UCadenceSequencerSection>(MovieSceneSection);
		FString SectionName = Section->GetSectionName();
		if(SectionName == InName)
		{
			Count++;
			if(Count >= 2)
				return true;
		}
	}

	return false;
}
