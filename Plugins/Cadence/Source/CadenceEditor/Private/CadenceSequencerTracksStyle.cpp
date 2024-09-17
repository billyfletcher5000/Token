// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSequencerTracksStyle.h"

#include "SequencerTrack/CadenceSequencerTrack.h"
#include "Styling/SlateStyleRegistry.h"

FName FCadenceSequencerTracksStyle::StyleName("CadenceSequencerTracksStyle");

FCadenceSequencerTracksStyle::FCadenceSequencerTracksStyle()
: FSlateStyleSet(StyleName)
{
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FCadenceSequencerTracksStyle::~FCadenceSequencerTracksStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

FCadenceSequencerTracksStyle& FCadenceSequencerTracksStyle::Get()
{
	static FCadenceSequencerTracksStyle Inst;
	return Inst;
}

void FCadenceSequencerTracksStyle::RegisterNewTrackType(TSubclassOf<UCadenceSequencerTrack> TrackType)
{
	UClass* Class = TrackType.Get();
	if (Class)
	{
		UCadenceSequencerTrack* CDO = Cast<UCadenceSequencerTrack>(Class->GetDefaultObject());
		Set(Class->GetFName(), new FSlateBrush(CDO->Icon));
	}
}
