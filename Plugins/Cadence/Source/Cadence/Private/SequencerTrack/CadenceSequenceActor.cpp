// Fill out your copyright notice in the Description page of Project Settings.


#include "SequencerTrack/CadenceSequenceActor.h"

#include "Cadence.h"
#include "CadenceSubsystem.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"


void ACadenceSequenceActor::PostInitializeComponents()
{
	if(CadenceGraph && CadenceGraph->GetGraph())
		LevelSequenceAsset = CadenceGraph->GetGraph()->Sequence;
	
	if(GetWorld()->IsGameWorld())
	{
		ULevelSequencePlayer* Player = GetSequencePlayer();
		Player->OnPlay.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStarted);
		Player->OnStop.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStopped);
		Player->OnFinished.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStopped);
		Player->OnSequenceUpdated().AddUObject(this, &ACadenceSequenceActor::OnPlayerUpdated);
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
