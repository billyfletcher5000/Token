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


DECLARE_MULTICAST_DELEGATE_OneParam(FCadenceSectionDelegate, FString InSectionName);

USTRUCT()
struct FCadenceSectionTimingData
{
	GENERATED_BODY()

	UPROPERTY()
	FString SectionName;

	UPROPERTY()
	float StartTime;

	UPROPERTY()
	float EndTime;

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
	
	void SetRunner(UCadenceGraphRunner* InRunner);
	UCadenceGraphRunner* GetRunner() const { return Runner; }

	void NotifySequenceUpdated(FFrameTime InCurrentTime);
	void NotifySequenceComplete() { bSequenceComplete = true; }	
	void NotifyRunnerComplete() { Runner = nullptr; bRunnerComplete = true; }

	bool IsSequenceComplete() const { return bSequenceComplete; }
	bool IsRunnerComplete() const { return bRunnerComplete; }
	bool IsInstanceComplete() const { return bSequenceComplete && bRunnerComplete; }

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
	TObjectPtr<UCadenceGraphRunner> Runner;

	UPROPERTY()
	bool bSequenceComplete = false;

	UPROPERTY()
	bool bRunnerComplete = false;

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

	
	UCadenceAssetInstance* GetOrCreateActiveAssetData(UCadenceAsset* InAsset);
	UCadenceAssetInstance* GetActiveAssetData(UCadenceAsset* InAsset);
	UCadenceAssetInstance* GetActiveAssetData(ULevelSequence* InSequence);
	UCadenceAssetInstance* GetActiveAssetData(UCadenceGraphRunner* InRunner);
	UCadenceGraphRunner* CreateRunner(UCadenceAssetInstance* AssetInstance);
	void NotifyGraphComplete(UCadenceGraphRunner* InRunner);

	static void LogOuterRelationships(UCadenceGraph* Copy, UCadenceGraph* Source);

private:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceAssetInstance>> ActiveAssets;

	UPROPERTY()
	TArray<UCadenceGraphRunner*> EndedRunners;
};
