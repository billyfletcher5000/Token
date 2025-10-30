// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "AudioVisualSubsystem.generated.h"

class UAVTriggerDataAsset;
class UAVTriggerInstance;
class UAVDurationSourceDataAsset;
class IAVDurationSource;
/**
 * 
 */
UCLASS()
class TOKEN_API UAudioVisualSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	// Begin UTickableWorldSubsystem
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// End UTickableWorldSubsystem

	IAVDurationSource* CreateDurationSource(UAVDurationSourceDataAsset* DataAsset);
	UAVTriggerInstance* CreateTriggerInstance(UAVTriggerDataAsset* DataAsset);

	UQuartzClockHandle* GetQuartzClockHandle() { return QuartzClockHandle; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoodTrackSectionDelegate, const FGuid&, MoodTrackSectionGUID);
	FMoodTrackSectionDelegate OnMoodTrackSectionHit;

private:
	void RegisterDurationSources();
	void RegisterTriggerInstances();


private:
	UQuartzClockHandle* QuartzClockHandle;

	TMap<FString, TFunction<IAVDurationSource* (UAVDurationSourceDataAsset*)>> DurationSourceFactoryMap;
	TMap<FString, TFunction<UAVTriggerInstance* ()>> TriggerFactoryMap;
};
