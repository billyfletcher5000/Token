// Fill out your copyright notice in the Description page of Project Settings.


#include "SequencerTrack/CadenceSequenceActor.h"

#include "Cadence.h"
#include "CadenceSubsystem.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "SequencerTrack/CadenceSequencerTrack.h"


void ACadenceSequenceActor::PostInitializeComponents()
{
	if(CadenceGraph && CadenceGraph->GetPrimaryGraph())
		LevelSequenceAsset = CadenceGraph->GetPrimaryGraph()->Sequence;
	
	if(GetWorld()->IsGameWorld())
	{
		ULevelSequencePlayer* Player = GetSequencePlayer();
		Player->OnPlay.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStarted);
		Player->OnStop.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStopped);
		Player->OnFinished.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStopped);
		Player->OnSequenceUpdated().AddUObject(this, &ACadenceSequenceActor::OnPlayerUpdated);
	}


	auto tracks = LevelSequenceAsset->GetMovieScene()->GetTracks();
	for(auto track : tracks)
	{
		if (UCadenceSequencerTrack* CadenceTrack = Cast<UCadenceSequencerTrack>(track))
		{
			auto Sections = CadenceTrack->GetAllSections();
			for(auto Section : Sections)
			{
				if (UCadenceSequencerSection* CadenceSection = Cast<UCadenceSequencerSection>(Section))
				{
					if (Sections.IndexOfByKey(Section) == 0)
					{
						
					}
				}
			}
		}
	}
	
	Super::PostInitializeComponents();
}

void ACadenceSequenceActor::OnPlayerStarted()
{
	UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
	if(ensure(Subsystem))
	{
		Subsystem->Notify_SequenceStart(CadenceGraph);
	}
}

void ACadenceSequenceActor::OnPlayerStopped()
{
	UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
	if(ensure(Subsystem))
	{
		Subsystem->Notify_SequenceEnd(CadenceGraph);
	}
}

void ACadenceSequenceActor::OnPlayerUpdated(const UMovieSceneSequencePlayer& Player, FFrameTime CurrentTime, FFrameTime PreviousTime)
{
	UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
	if(ensure(Subsystem))
	{
		Subsystem->Notify_SequenceUpdated(CadenceGraph, CurrentTime, PreviousTime);
	}
}
