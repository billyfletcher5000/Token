// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubsystem.h"

void UCadenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCadenceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UCadenceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TStatId UCadenceSubsystem::GetStatId() const
{	
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCadenceSubsystem, STATGROUP_Tickables);
}
