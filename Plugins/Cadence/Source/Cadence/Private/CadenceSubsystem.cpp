// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubsystem.h"

#include "CadenceAsset.h"
#include "CadenceGraphRunner.h"

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

	for(UCadenceGraphRunner* Runner : ActiveRunners)
		Runner->Tick(DeltaTime);

	for(UCadenceGraphRunner* Runner : EndedRunners)
		ActiveRunners.Remove(Runner);

	EndedRunners.Empty();
}

TStatId UCadenceSubsystem::GetStatId() const
{	
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCadenceSubsystem, STATGROUP_Tickables);
}

UCadenceGraphRunner* UCadenceSubsystem::ActivateGraph(UCadenceAsset* CadenceAsset)
{
	UCadenceGraphRunner* Runner = NewObject<UCadenceGraphRunner>();
	UCadenceContext* Context = NewObject<UCadenceContext>();
	Context->Graph = CadenceAsset->GetGraph();
	Runner->Init(Context);

	ActiveRunners.Add(Runner);
	Runner->Begin();

	return Runner;
}

void UCadenceSubsystem::NotifyGraphComplete(UCadenceGraphRunner* InRunner)
{
	EndedRunners.Add(InRunner);
}
