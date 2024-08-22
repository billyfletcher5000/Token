// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioVisualSubsystem.h"

// Duration Sources
#include "AVDurationSource.h"
#include "MoodSection/AVDurationSourceMoodSection.h"

void UAudioVisualSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RegisterDurationSources();
}

void UAudioVisualSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAudioVisualSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

IAVDurationSource* UAudioVisualSubsystem::CreateDurationSource(UAVDurationSourceDataAsset* DataAsset)
{
	const FString& DataAssetClassName = DataAsset->GetClass()->GetName();
	auto CreateFunc = DurationSourceFactoryMap.Find(DataAssetClassName);
	
	if(CreateFunc != nullptr)
	{
		return (*CreateFunc)(DataAsset);
	}

	return nullptr;
}

void UAudioVisualSubsystem::RegisterDurationSources()
{
	DurationSourceFactoryMap.Add(TNameOf<UAVDurationSourceMoodSectionDataAsset>::GetName(), &AVDurationSourceMoodSection::Create);
}
