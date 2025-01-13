// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphRunner.h"
#include "Graph/Nodes/CadenceDurationNodes.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "UObject/Object.h"
#include "CadenceSubsystem.generated.h"

class UCadenceAssetInstance;
class UCadenceSequencerSection;
class UMovieSceneSequence;
class UCadenceGraph;
class UCadenceAsset;
class UCadenceGraphRunner;
class UCadenceRunGraphTickable;

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

	UCadenceReactionGroup* GetReactionGroupRuntimeInstance(UCadenceReactionGroup* InGroupAsset);	

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

	UPROPERTY(Transient)
	TMap<UCadenceReactionGroup*, UCadenceReactionGroup*> ReactionGroupAssetToInstance;
};
