// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceAssetInstance.generated.h"

class UQuartzClockHandle;
enum class ECadenceSectionEdgeQuantizationType : uint8;
class UMovieSceneSection;
class UCadenceGraphRunner;
class UCadenceSequencerSection;
class UCadenceAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FCadenceSectionDelegate, FString InSectionName);

USTRUCT()
struct FCadenceSectionTimingData
{
	GENERATED_BODY()

	UPROPERTY()
	FString SectionName;

	UPROPERTY()
	float StartTime = 0.0f;

	UPROPERTY()
	float EndTime = 0.0f;

	UPROPERTY()
	bool bHasStarted = false;

	UPROPERTY()
	bool bHasEnded = false;

	float GetDuration() const { return EndTime - StartTime; }
};

UCLASS()
class UCadenceAssetInstance : public UObject
{
	GENERATED_BODY()

public:
	void Init(UCadenceAsset* InAsset);
	virtual void BeginDestroy() override;
	
	float GetAlignedTime(float TimeInSeconds, ECadenceSectionEdgeQuantizationType StartEdgeQuantizationType, EQuartzCommandQuantization QuartzCommandQuantization) const;
	float GetTimeUntilNextBoundary(ECadenceSectionEdgeQuantizationType StartEdgeQuantizationType, EQuartzCommandQuantization QuartzCommandQuantization, const float& InNegativeTimeThreshold = 0.0f) const;
	void GenerateSectionDurationData(UCadenceSequencerSection* InStartSection);
	float GetSectionDuration(const FString& SectionName) const;

	float GetQuantizationDuration(const EQuartzCommandQuantization& InDuration) const;
	
	UCadenceAsset* GetAsset() const { return Asset; }
	
	void SetPrimaryRunner(UCadenceGraphRunner* InRunner);
	UCadenceGraphRunner* GetPrimaryRunner() const { return PrimaryRunner; }

	void AddAdditionalRunner(UCadenceGraphRunner* InRunner);

	TArray<UCadenceGraphRunner*> GetAllRunners() const;
	
	void NotifySequenceUpdated(FFrameTime InCurrentTime);
	void NotifySequenceComplete() { bSequenceComplete = true; }	
	void NotifyRunnerComplete(UCadenceGraphRunner* InRunner);

	bool IsSequenceComplete() const { return bSequenceComplete; }
	bool IsPrimaryRunnerComplete() const { return bPrimaryRunnerComplete; }
	bool IsAdditionalRunnerComplete(UCadenceGraphRunner* InRunner) const;
	bool IsInstanceComplete() const { return bSequenceComplete && bPrimaryRunnerComplete; }

	FCadenceSectionDelegate OnSectionStarted;
	FCadenceSectionDelegate OnSectionEnded;
	
private:
	float GetStartFrameSeconds(UMovieSceneSection* Section) const;
	float GetEndFrameSeconds(UMovieSceneSection* Section) const;
	
private:	
	UPROPERTY()
	TObjectPtr<UCadenceAsset> Asset;
	
	UPROPERTY()
	TObjectPtr<UCadenceGraphRunner> PrimaryRunner;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphRunner>> AdditionalRunners;
	
	UPROPERTY()
	bool bSequenceComplete = false;

	UPROPERTY()
	bool bPrimaryRunnerComplete = false;

	UPROPERTY()
	TArray<FCadenceSectionTimingData> TimingDataList;

	UPROPERTY()
	TObjectPtr<UQuartzClockHandle> ClockHandle;
};