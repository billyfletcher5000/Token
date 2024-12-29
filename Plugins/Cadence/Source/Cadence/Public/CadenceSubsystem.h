// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphRunner.h"
#include "Graph/Nodes/CadenceDurationNodes.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "UObject/Object.h"
#include "CadenceSubsystem.generated.h"

class UCadenceSequencerTrack;
class UCadenceSequencerSection;
class UMovieSceneSequence;
class UCadenceGraph;
class UCadenceAsset;
class UCadenceGraphRunner;
class UQuartzClockHandle;
class UCadenceRunGraphTickable;


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
	
	void GenerateSectionDurationData(UCadenceSequencerSection* InStartSection);
	float GetSectionDuration(const FString& SectionName);

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
	float GetAlignedTime(float TimeInSeconds, ECadenceSectionEdgeQuantizationType StartEdgeQuantizationType, EQuartzCommandQuantization QuartzCommandQuantization) const;
	float GetStartFrameSeconds(UMovieSceneSection* Section);
	float GetEndFrameSeconds(UMovieSceneSection* Section);
	
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

UCLASS()
class CADENCE_API UCadenceSubsystem : public UTickableWorldSubsystem
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
	

	UFUNCTION(BlueprintCallable)
	void Notify_SectionStart(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SectionEnd(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SequenceStart(UCadenceAsset* CadenceAsset);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SequenceEnd(UCadenceAsset* CadenceAsset);

	void Notify_SequenceUpdated(UCadenceAsset* CadenceAsset, FFrameTime CurrentTime, FFrameTime PreviousTime);

protected:
	friend UCadenceGraphRunner;
	friend UCadenceRunGraphTickable;
	
	UCadenceAssetInstance* GetOrCreateActiveAssetData(UCadenceAsset* InAsset);
	UCadenceAssetInstance* GetActiveAssetData(UCadenceAsset* InAsset);
	UCadenceAssetInstance* GetActiveAssetData(ULevelSequence* InSequence);
	UCadenceAssetInstance* GetActiveAssetData(UCadenceGraphRunner* InRunner);
	UCadenceGraphRunner* CreateRunner(UCadenceAssetInstance* InAssetInstance, UCadenceGraph* InGraph = nullptr);
	void NotifyGraphComplete(UCadenceGraphRunner* InRunner);

	static void LogOuterRelationships(UCadenceGraph* Copy, UCadenceGraph* Source);

private:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceAssetInstance>> ActiveAssets;

	UPROPERTY()
	TArray<UCadenceGraphRunner*> EndedRunners;
};
