// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubsystem.h"

#include "Cadence.h"
#include "CadenceAsset.h"
#include "CadenceAssetInstance.h"
#include "CadenceContext.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadenceGraphRunner.h"
#include "SequencerTrack/CadenceSequencerSection.h"

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

	for(UCadenceAssetInstance* AssetInstance : ActiveAssets)
	{
		TArray<UCadenceGraphRunner*> Runners = AssetInstance->GetAllRunners();
		for(UCadenceGraphRunner* Runner : Runners)
		{			
			if(IsValid(Runner))
				Runner->Tick(DeltaTime);
		}
	}

	for(UCadenceGraphRunner* Runner : EndedRunners)
	{		
		if(UCadenceAssetInstance* Data = GetActiveAssetData(Runner))
		{
			Data->NotifyRunnerComplete(Runner);
			if(Data->IsInstanceComplete())
				ActiveAssets.Remove(Data);			
		}
	}

	EndedRunners.Empty();
}

TStatId UCadenceSubsystem::GetStatId() const
{	
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCadenceSubsystem, STATGROUP_Tickables);
}

UCadenceReactionGroup* UCadenceSubsystem::GetReactionGroupRuntimeInstance(UCadenceReactionGroup* InGroupAsset)
{
	if(ReactionGroupAssetToInstance.Contains(InGroupAsset))
		return ReactionGroupAssetToInstance[InGroupAsset];

	FString Name = InGroupAsset->GetFName().ToString() + "_Inst";
	UCadenceReactionGroup* Instance = DuplicateObject<UCadenceReactionGroup>(InGroupAsset, this, FName(Name));

	ReactionGroupAssetToInstance.Add(InGroupAsset, Instance);
	return Instance;
}

UCadenceGraphRunner* UCadenceSubsystem::CreateRunner(UCadenceAssetInstance* InAssetInstance, UCadenceGraph* InGraph)
{	
	UCadenceGraphRunner* Runner = NewObject<UCadenceGraphRunner>(this);
	UCadenceContext* Context = NewObject<UCadenceContext>(Runner);
	TObjectPtr<UCadenceGraph> Graph = IsValid(InGraph) ? InGraph : InAssetInstance->GetAsset()->GetPrimaryGraph();
	Context->SourceGraph = Graph;
	Context->Graph = DuplicateObject(Graph, Context, "Graph");
	Context->Asset = InAssetInstance->GetAsset();
	Context->AssetInstance = InAssetInstance;
	Context->ActorLifetimeManager = NewObject<UCadenceActorLifetimeManager>(Context);

	LogOuterRelationships(Context->Graph, Graph);
	
	Runner->Init(Context);

	return Runner;
}

void UCadenceSubsystem::Notify_SectionStart(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section)
{
	UE_LOG(LogCadence, Log, TEXT("Section Start: Sequence: %s Section: %s"), *Sequence->GetName(), *Section->GetName());

	ULevelSequence* LevelSequence = Cast<ULevelSequence>(Sequence);
	UCadenceAssetInstance* Data = GetActiveAssetData(LevelSequence);
	
	if(!Data->IsPrimaryRunnerComplete() && Data->GetPrimaryRunner() == nullptr)
	{
		Data->GenerateSectionDurationData(Section);
		UCadenceGraphRunner* Runner = CreateRunner(Data);
		Data->SetPrimaryRunner(Runner);
		Runner->Begin();
	}
}

void UCadenceSubsystem::Notify_SectionEnd(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section)
{
	UE_LOG(LogCadence, Log, TEXT("Section End: Sequence: %s Section: %s"), *Sequence->GetName(), *Section->GetName());
}

void UCadenceSubsystem::Notify_SequenceStart(UCadenceAsset* CadenceAsset)
{
	UE_LOG(LogCadence, Log, TEXT("Sequence Start: %s"), *CadenceAsset->GetName());

	GetOrCreateActiveAssetData(CadenceAsset);
}

void UCadenceSubsystem::Notify_SequenceEnd(UCadenceAsset* CadenceAsset)
{
	UE_LOG(LogCadence, Log, TEXT("Sequence End: %s"), *CadenceAsset->GetName());

	UCadenceAssetInstance* Data = GetActiveAssetData(CadenceAsset);
	if(Data)
	{
		Data->NotifySequenceComplete();
		if(Data->IsInstanceComplete())
			ActiveAssets.Remove(Data);
	}
}

void UCadenceSubsystem::Notify_SequenceUpdated(UCadenceAsset* CadenceAsset, FFrameTime CurrentTime,	FFrameTime PreviousTime)
{
	UCadenceAssetInstance* Data = GetActiveAssetData(CadenceAsset);
	if(Data)
	{
		Data->NotifySequenceUpdated(CurrentTime);
	}
}

UCadenceAssetInstance* UCadenceSubsystem::GetOrCreateActiveAssetData(UCadenceAsset* InAsset)
{
	UCadenceAssetInstance* Data = GetActiveAssetData(InAsset);
	if (Data)
		return Data;

	UCadenceAssetInstance* AssetInstance = NewObject<UCadenceAssetInstance>(this);
	AssetInstance->Init(InAsset);
	ActiveAssets.Add(AssetInstance);
	
	return AssetInstance;
}

UCadenceAssetInstance* UCadenceSubsystem::GetActiveAssetData(UCadenceAsset* InAsset)
{
	for(UCadenceAssetInstance* AssetInstance : ActiveAssets)
	{
		if(AssetInstance->GetAsset() == InAsset)
		{
			return AssetInstance;
		}
	}
	
	return nullptr;
}

UCadenceAssetInstance* UCadenceSubsystem::GetActiveAssetData(ULevelSequence* InSequence)
{
	for(UCadenceAssetInstance* AssetInstance : ActiveAssets)
	{
		if(AssetInstance->GetAsset()->GetPrimaryGraph()->GetSequence() == InSequence)
		{			
			return AssetInstance;
		}
	}
	
	return nullptr;
}

UCadenceAssetInstance* UCadenceSubsystem::GetActiveAssetData(UCadenceGraphRunner* InRunner)
{
	for(UCadenceAssetInstance* AssetInstance : ActiveAssets)
	{
		if(AssetInstance->GetAllRunners().Contains(InRunner))
		{			
			return AssetInstance;
		}
	}
	
	return nullptr;
}

void UCadenceSubsystem::NotifyGraphComplete(UCadenceGraphRunner* InRunner)
{
	EndedRunners.Add(InRunner);
}

#define PB(b) b ? *FString("true") : *FString("false")

void UCadenceSubsystem::LogOuterRelationships(UCadenceGraph* Copy, UCadenceGraph* Source)
{
	UE_LOG(LogCadence, Log, TEXT("Copy Pointer: %p  |  Source Pointer: %p"), (void*)Copy, (void*)Source);

	for(auto Node : Copy->Nodes)
	{
		bool OuterIsCopy = Node->IsInOuter(Copy);
		bool OuterIsSource = Node->IsInOuter(Source);
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
