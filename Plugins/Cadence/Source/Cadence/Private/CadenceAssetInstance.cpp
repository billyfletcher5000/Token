// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceAssetInstance.h"

#include "Cadence.h"
#include "MovieScene.h"
#include "MovieSceneTimeHelpers.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphRunner.h"
#include "Quartz/AudioMixerClockHandle.h"
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
	UCadenceGraph* Graph = Asset->GetPrimaryGraph();
	
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

void UCadenceAssetInstance::SetPrimaryRunner(UCadenceGraphRunner* InRunner)
{
	if(PrimaryRunner != InRunner)
	{
		PrimaryRunner = InRunner;
		bPrimaryRunnerComplete = PrimaryRunner == nullptr;
		PrimaryRunner->GetContext()->AssetInstance = this; //TODO: Restructure this
	}	
}

void UCadenceAssetInstance::AddAdditionalRunner(UCadenceGraphRunner* InRunner)
{
	if(ensure(IsValid(InRunner)))
		AdditionalRunners.AddUnique(InRunner);
}

TArray<UCadenceGraphRunner*> UCadenceAssetInstance::GetAllRunners() const
{
	// Turbo floyd??? Truth Magnum???
	TArray<UCadenceGraphRunner*> OutRunners = { PrimaryRunner };
	
	OutRunners.Append(AdditionalRunners);

	return OutRunners;
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

void UCadenceAssetInstance::NotifyRunnerComplete(UCadenceGraphRunner* InRunner)
{
	if(InRunner == PrimaryRunner)
	{
		PrimaryRunner = nullptr; bPrimaryRunnerComplete = true;
	}
	else
	{
		AdditionalRunners.Remove(InRunner);		
	}
}

bool UCadenceAssetInstance::IsAdditionalRunnerComplete(UCadenceGraphRunner* InRunner) const
{
	return !AdditionalRunners.Contains(InRunner);
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