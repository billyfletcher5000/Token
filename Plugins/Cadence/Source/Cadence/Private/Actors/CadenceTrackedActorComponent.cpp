// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceTrackedActorComponent.h"

#include "Cadence.h"
#include "CadenceSubsystem.h"


void UCadenceTrackedActorComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UCadenceTrackedActorComponent::PostLoad()
{
	Super::PostLoad();

	if(!TrackedActorID.IsValid())
	{
		if(UWorld* World = this->GetWorld())
		{
			UE_LOG(LogCadence, Log, TEXT("Regenerating GUID: %d"), (int32)World->WorldType);
			RegenerateCadenceGUID();
		}
	}
}

void UCadenceTrackedActorComponent::RegenerateCadenceGUID()
{
	Modify();
	TrackedActorID = FGuid::NewDeterministicGuid(this->GetPathName());
}

void UCadenceTrackedActorComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(!ensure(World))	
		return;

	UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!ensure(CadenceSubsystem))
		return;

	CadenceSubsystem->RegisterTrackedActor(GetOwner(), TrackedActorID);
}

void UCadenceTrackedActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if(!ensure(World))	
		return;

	UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!ensure(CadenceSubsystem))
		return;

	CadenceSubsystem->UnregisterTrackedActor(GetOwner(), TrackedActorID);

	Super::EndPlay(EndPlayReason);
}
