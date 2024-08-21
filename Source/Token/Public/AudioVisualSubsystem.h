// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AudioVisualSubsystem.generated.h"

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
	// End UTickableWorldSubsystem

	TObjectPtr<IAVDurationSource> CreateDurationSource(UAVDurationSourceDataAsset* DataAsset);

private:
	void RegisterDurationSources();

	TMap<FString, TFunction<TObjectPtr<IAVDurationSource>(UAVDurationSourceDataAsset*)>> DurationSourceFactoryMap;
};
