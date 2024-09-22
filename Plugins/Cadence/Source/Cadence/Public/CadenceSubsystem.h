// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraph.h"
#include "UObject/Object.h"
#include "CadenceSubsystem.generated.h"

class UCadenceSequencerSection;
class UMovieSceneSequence;
class UCadenceGraph;
class UCadenceAsset;
class UCadenceGraphRunner;

/**
 * 
 */
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
	UCadenceGraphRunner* ActivateGraph(UCadenceAsset* CadenceAsset);

	UFUNCTION(BlueprintCallable)
	void Notify_SectionStart(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SectionEnd(UMovieSceneSequence* Sequence, UCadenceSequencerSection* Section);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SequenceStart(UCadenceAsset* CadenceAsset);
	
	UFUNCTION(BlueprintCallable)
	void Notify_SequenceEnd(UCadenceAsset* CadenceAsset);

protected:
	friend UCadenceGraphRunner;

	void NotifyGraphComplete(UCadenceGraphRunner* InRunner);

	static void LogOuterRelationships(UCadenceGraph* Copy, UCadenceGraph* Source);

private:
	UPROPERTY()
	TArray<UCadenceGraphRunner*> ActiveRunners;

	UPROPERTY()
	TArray<UCadenceGraphRunner*> EndedRunners;
};
