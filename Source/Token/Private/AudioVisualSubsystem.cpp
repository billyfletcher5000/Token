// Copyright (C) Billy Fletcher 2025


#include "AudioVisualSubsystem.h"

// Duration Sources
#include "AVDurationSource.h"
#include "MoodSection/AVDurationSourceMoodSection.h"

// Triggers
#include "AVTrigger.h"
#include "MoodSection/AVTriggerMoodSection.h"

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

TStatId UAudioVisualSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAudioVisualSubsystem, STATGROUP_Tickables);
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

UAVTriggerInstance* UAudioVisualSubsystem::CreateTriggerInstance(UAVTriggerDataAsset* DataAsset)
{
	const FString& DataAssetClassName = DataAsset->GetClass()->GetName();
	auto CreateFunc = TriggerFactoryMap.Find(DataAssetClassName);
	
	if(CreateFunc != nullptr)
	{
		UAVTriggerInstance* Instance = (*CreateFunc)();
		if(ensure(Instance))
		{
			Instance->Init(DataAsset);
			return Instance;
		}
	}

	return nullptr;
}

void UAudioVisualSubsystem::RegisterDurationSources()
{
	DurationSourceFactoryMap.Add(TNameOf<UAVDurationSourceMoodSectionDataAsset>::GetName(), &AVDurationSourceMoodSection::Create);
}

void UAudioVisualSubsystem::RegisterTriggerInstances()
{
	TriggerFactoryMap.Add(TNameOf<UAVTriggerMoodSectionDataAsset>::GetName(), &UAVTriggerMoodSectionInstance::Create);
}
