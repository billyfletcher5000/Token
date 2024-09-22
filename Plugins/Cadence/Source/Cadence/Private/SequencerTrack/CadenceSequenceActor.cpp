// Fill out your copyright notice in the Description page of Project Settings.


#include "SequencerTrack/CadenceSequenceActor.h"

#include "CadenceSubsystem.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"


void ACadenceSequenceActor::PostInitializeComponents()
{
	if(CadenceGraph && CadenceGraph->GetGraph())
		LevelSequenceAsset = CadenceGraph->GetGraph()->Sequence;
	
	if(GetWorld()->IsGameWorld())
	{
		ULevelSequencePlayer* Player = GetSequencePlayer();
		Player->OnPlay.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStarted);
		Player->OnStop.AddUniqueDynamic(this, &ACadenceSequenceActor::OnPlayerStopped);
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