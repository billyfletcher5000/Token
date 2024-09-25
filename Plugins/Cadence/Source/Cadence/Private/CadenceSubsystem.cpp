// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubsystem.h"

#include "Cadence.h"
#include "CadenceAsset.h"
#include "CadenceContext.h"
#include "MovieScene.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadenceGraphRunner.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Quartz/QuartzSubsystem.h"
#include "SequencerTrack/CadenceSequencerSection.h"

void UCadenceAssetInstance::Init(UCadenceAsset* InAsset)
{
	Asset = InAsset;
}

void UCadenceAssetInstance::BeginDestroy()
{
	UObject::BeginDestroy();
	
	if(IsValid(ClockHandle))
	{
		if(UQuartzSubsystem* QuartzSubsystem = GetWorld()->GetSubsystem<UQuartzSubsystem>())
		{
			UQuartzClockHandle* Handle = ClockHandle;
			QuartzSubsystem->DeleteClockByHandle(this, Handle);
		}
	}
}

void UCadenceAssetInstance::GenerateSectionDurationData(UCadenceSequencerSection* InStartSection)
{
	UCadenceGraph* Graph = Asset->GetGraph();
	
	UQuartzSubsystem* QuartzSubsystem = GetWorld()->GetSubsystem<UQuartzSubsystem>();
	FQuartzClockSettings Settings;	
	Settings.TimeSignature = Graph->GetTimeSignature();
	UQuartzClockHandle* NewClockHandle = QuartzSubsystem->CreateNewClock(this, FName(Asset->GetName()), Settings);

	FQuartzQuantizationBoundary Boundary;
	FOnQuartzCommandEventBP EmptyDelegate;	
	
	NewClockHandle->SetBeatsPerMinute(this, Boundary, EmptyDelegate, NewClockHandle, Graph->GetBPM());
	NewClockHandle->StartClock(this, NewClockHandle);
	
	ClockHandle = NewClockHandle;
	
	TimingDataList.Empty();

	float StartOffsetSeconds = GetStartFrameSeconds(InStartSection);
	TArray<UCadenceSequencerSection*> Sections = Graph->GetSections();
			
	for(UCadenceSequencerSection* Section : Sections)
	{
		FCadenceSectionTimingData TimingData;
		float StartFrameSeconds = GetStartFrameSeconds(Section) - StartOffsetSeconds;
		float EndFrameSeconds = GetEndFrameSeconds(Section) - StartOffsetSeconds;

		TimingData.SectionName = Section->GetSectionName();
		TimingData.StartTime = GetAlignedTime(StartFrameSeconds, Section->StartEdgeQuantizationType, Section->StartEdgeQuantizationBoundary);
		TimingData.EndTime = GetAlignedTime(EndFrameSeconds, Section->EndEdgeQuantizationType, Section->EndEdgeQuantizationBoundary);

		TimingDataList.Add(TimingData);		
	}
}

float UCadenceAssetInstance::GetSectionDuration(const FString& SectionName)
{
	for(FCadenceSectionTimingData& Data : TimingDataList)
	{
		if(Data.SectionName == SectionName)
			return Data.GetDuration();
	}

	UE_LOG(LogCadence, Warning, TEXT("Cannot retrieve section duration for unrecognised section name: %s"), *SectionName);
	return 0.0f;
}

void UCadenceAssetInstance::SetRunner(UCadenceGraphRunner* InRunner)
{
	if(Runner != InRunner)
	{
		Runner = InRunner;
		bRunnerComplete = Runner == nullptr;
		Runner->GetContext()->AssetInstance = this; //TODO: Restructure this
	}	
}

void UCadenceAssetInstance::NotifySequenceUpdated(FFrameTime InCurrentTime)
{
	float Time = InCurrentTime.AsDecimal();
	for(FCadenceSectionTimingData& Data : TimingDataList)
	{
		if(!Data.bHasStarted && Time > Data.StartTime)
		{
			Data.bHasStarted = true;
			OnSectionStarted.Broadcast(Data.SectionName);
		}
		else if(!Data.bHasEnded && Time > Data.EndTime)
		{
			Data.bHasEnded = true;
			OnSectionEnded.Broadcast(Data.SectionName);
		}
	}
}

float UCadenceAssetInstance::GetAlignedTime(float TimeInSeconds, ECadenceSectionEdgeQuantizationType EdgeQuantizationType, EQuartzCommandQuantization QuantizationBoundary) const
{
	if(EdgeQuantizationType == ECadenceSectionEdgeQuantizationType::NoQuantization)
		return TimeInSeconds;

	float QuantizationBoundarySeconds = ClockHandle->GetDurationOfQuantizationTypeInSeconds(this, QuantizationBoundary);
	
	float CurrentSeconds = 0.0f;
	while (CurrentSeconds < TimeInSeconds)
	{
		float NextBoundaryPoint = CurrentSeconds + QuantizationBoundarySeconds;
		if(NextBoundaryPoint == TimeInSeconds)
			return TimeInSeconds;

		if(NextBoundaryPoint > TimeInSeconds)
		{
			if(EdgeQuantizationType == ECadenceSectionEdgeQuantizationType::Before)
				return CurrentSeconds;

			if(EdgeQuantizationType == ECadenceSectionEdgeQuantizationType::After)
				return NextBoundaryPoint;

			return (TimeInSeconds - CurrentSeconds) < (NextBoundaryPoint - TimeInSeconds) ? CurrentSeconds : NextBoundaryPoint;
		}
		
		CurrentSeconds = NextBoundaryPoint;
	}

	return TimeInSeconds;
}

float UCadenceAssetInstance::GetStartFrameSeconds(UMovieSceneSection* Section)
{
	if (!Section)
	{
		FFrame::KismetExecutionMessage(TEXT("Cannot call GetStartFrameSeconds on a null section"), ELogVerbosity::Error);
		return -1.f;
	}

	if (!Section->HasStartFrame())
	{
		FFrame::KismetExecutionMessage(TEXT("Section does not have a start frame"), ELogVerbosity::Error);
		return -1.f;
	}

	UMovieScene* MovieScene = Section->GetTypedOuter<UMovieScene>();
	if (MovieScene)
	{
		FFrameRate DisplayRate = MovieScene->GetDisplayRate();
		return DisplayRate.AsSeconds(ConvertFrameTime(UE::MovieScene::DiscreteInclusiveLower(Section->GetRange()), MovieScene->GetTickResolution(), DisplayRate));
	}

	return -1.f;
}

float UCadenceAssetInstance::GetEndFrameSeconds(UMovieSceneSection* Section)
{
	if (!Section)
	{
		FFrame::KismetExecutionMessage(TEXT("Cannot call GetEndFrameSeconds on a null section"), ELogVerbosity::Error);
		return -1.f;
	}

	if (!Section->HasEndFrame())
	{
		FFrame::KismetExecutionMessage(TEXT("Section does not have an end frame"), ELogVerbosity::Error);
		return -1.f;
	}

	UMovieScene* MovieScene = Section->GetTypedOuter<UMovieScene>();
	if (MovieScene)
	{
		FFrameRate DisplayRate = MovieScene->GetDisplayRate();
		return DisplayRate.AsSeconds(ConvertFrameTime(UE::MovieScene::DiscreteExclusiveUpper(Section->GetRange()), MovieScene->GetTickResolution(), DisplayRate));
	}

	return -1.f;	
}

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
		UCadenceGraphRunner* Runner = AssetInstance->GetRunner();
		if(IsValid(Runner))
			Runner->Tick(DeltaTime);
	}

	for(UCadenceGraphRunner* Runner : EndedRunners)
	{		
		if(UCadenceAssetInstance* Data = GetActiveAssetData(Runner))
		{
			Data->NotifyRunnerComplete();
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

UCadenceGraphRunner* UCadenceSubsystem::CreateRunner(UCadenceAssetInstance* AssetInstance)
{	
	UCadenceGraphRunner* Runner = NewObject<UCadenceGraphRunner>(this);
	UCadenceContext* Context = NewObject<UCadenceContext>(Runner);
	TObjectPtr<UCadenceGraph> Graph = AssetInstance->GetAsset()->GetGraph();
	Context->SourceGraph = Graph;
	Context->Graph = DuplicateObject(Graph, Context, "Graph");
	Context->Asset = AssetInstance->GetAsset();
	Context->AssetInstance = AssetInstance;
	Context->ActorLifetimeManager = NewObject<UCadenceActorLifetimeManager>(Context);

	LogOuterRelationships(Context->Graph, Graph);
	
	Runner->Init(Context);
	Runner->Begin();

	return Runner;
}

void UCadenceSubsystem::Notify_SectionStart(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section)
{
	UE_LOG(LogCadence, Log, TEXT("Section Start: Sequence: %s Section: %s"), *Sequence->GetName(), *Section->GetName());

	ULevelSequence* LevelSequence = Cast<ULevelSequence>(Sequence);
	UCadenceAssetInstance* Data = GetActiveAssetData(LevelSequence);
	
	if(!Data->IsRunnerComplete() && Data->GetRunner() == nullptr)
	{
		Data->GenerateSectionDurationData(Section);
		Data->SetRunner(CreateRunner(Data));
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
		if(AssetInstance->GetAsset()->GetGraph()->GetSequence() == InSequence)
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
		if(AssetInstance->GetRunner() == InRunner)
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
