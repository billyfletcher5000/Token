// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubsystem.h"

#include "Cadence.h"
#include "CadenceAsset.h"
#include "CadenceContext.h"
#include "CadenceSettings.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadenceGraphRunner.h"
#include "Logging/StructuredLog.h"

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
	UCadenceGraphRunner* Runner = NewObject<UCadenceGraphRunner>(this);
	UCadenceContext* Context = NewObject<UCadenceContext>(Runner);
	TObjectPtr<UCadenceGraph> Graph = CadenceAsset->GetGraph();
	Context->SourceGraph = Graph;
	Context->Graph = DuplicateObject(Graph, Context, "Graph");
	Context->Asset = CadenceAsset;
	Context->ActorLifetimeManager = NewObject<UCadenceActorLifetimeManager>();

	LogOuterRelationships(Context->Graph, Graph);
	
	Runner->Init(Context);

	ActiveRunners.Add(Runner);
	Runner->Begin();

	return Runner;
}

void UCadenceSubsystem::NotifyGraphComplete(UCadenceGraphRunner* InRunner)
{
	EndedRunners.Add(InRunner);
}

#define PB(b) b ? *FString("true") : *FString("false")

void UCadenceSubsystem::LogOuterRelationships(UCadenceGraph* Copy, UCadenceGraph* Source)
{
	UE_LOG(LogCadence, Log, TEXT("Copy Pointer: %p  |  Source Pointer: %p"), (void*)Copy, (void*)Source);

	bool OuterIsCopy = false;
	bool OuterIsSource = false;
	
	for(auto Node : Copy->Nodes)
	{
		OuterIsCopy = Node->IsInOuter(Copy);
		OuterIsSource = Node->IsInOuter(Source);
		UE_LOG(LogCadence, Log, TEXT("Node: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *Node->GetName(), (void*)Node, PB(OuterIsCopy), PB(OuterIsSource));

		for(auto InputPin : Node->GetInputPins())
		{
			OuterIsCopy = InputPin->IsInOuter(Copy);
			OuterIsSource = InputPin->IsInOuter(Source);
			UE_LOG(LogCadence, Log, TEXT("\tInputPin: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *InputPin->GetName(), (void*)InputPin, PB(OuterIsCopy), PB(OuterIsSource));

			if(UCadenceVariable* Variable = InputPin->GetVariable(false))
			{
				OuterIsCopy = Variable->IsInOuter(Copy);
				OuterIsSource = Variable->IsInOuter(Source);
				UE_LOG(LogCadence, Log, TEXT("\t\tVariable: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *Variable->GetName(), (void*)Variable, PB(OuterIsCopy), PB(OuterIsSource));
			}

			for(auto ConnectedPin : InputPin->GetConnectedPins())
			{
				OuterIsCopy = ConnectedPin->IsInOuter(Copy);
				OuterIsSource = ConnectedPin->IsInOuter(Source);
				UE_LOG(LogCadence, Log, TEXT("\t\tConnectedPin: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *ConnectedPin->GetName(), (void*)ConnectedPin, PB(OuterIsCopy), PB(OuterIsSource));
			}
		}

		for(auto OutputPin : Node->GetInputPins())
		{
			OuterIsCopy = OutputPin->IsInOuter(Copy);
			OuterIsSource = OutputPin->IsInOuter(Source);
			UE_LOG(LogCadence, Log, TEXT("\tOutputPin: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *OutputPin->GetName(), (void*)OutputPin, PB(OuterIsCopy), PB(OuterIsSource));

			if(UCadenceVariable* Variable = OutputPin->GetVariable(false))
			{
				OuterIsCopy = Variable->IsInOuter(Copy);
				OuterIsSource = Variable->IsInOuter(Source);
				UE_LOG(LogCadence, Log, TEXT("\t\tVariable: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *Variable->GetName(), (void*)Variable, PB(OuterIsCopy), PB(OuterIsSource));
			}

			for(auto ConnectedPin : OutputPin->GetConnectedPins())
			{
				OuterIsCopy = ConnectedPin->IsInOuter(Copy);
				OuterIsSource = ConnectedPin->IsInOuter(Source);
				UE_LOG(LogCadence, Log, TEXT("\t\tConnectedPin: Name: %s Pointer: %p OuterIsCopy: %s OuterIsSource: %s"), *ConnectedPin->GetName(), (void*)ConnectedPin, PB(OuterIsCopy), PB(OuterIsSource));
			}
		}
	}
}
