// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstance.h"
#include "CadenceSequencerTrackInstance.generated.h"

class UCadenceSequencerSection;

USTRUCT(BlueprintType)
struct FCadenceSequencerTrackInstanceInput
{
	GENERATED_BODY()

	FCadenceSequencerTrackInstanceInput()
		: Section(nullptr)
		, Context(FMovieSceneEvaluationRange(0, FFrameRate()))
	{}

	UPROPERTY(Category="Sequencer", BlueprintReadOnly)
	TObjectPtr<UCadenceSequencerSection> Section;

	FMovieSceneContext Context;
};


UCLASS(Blueprintable, DisplayName=CadenceSequencerTrackInstance)
class CADENCE_API UCadenceSequencerTrackInstance : public UMovieSceneTrackInstance
{
public:

	GENERATED_BODY()

	/*~ Implementable interface */

	UFUNCTION(Category="Sequencer", DisplayName="OnInitialize", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnInitialize();

	UFUNCTION(Category="Sequencer", DisplayName="OnUpdate", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnUpdate();

	UFUNCTION(Category="Sequencer", DisplayName="OnBeginUpdateInputs", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnBeginUpdateInputs();

	UFUNCTION(Category="Sequencer", DisplayName="OnInputAdded", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnInputAdded(FCadenceSequencerTrackInstanceInput Input);

	UFUNCTION(Category="Sequencer", DisplayName="OnInputRemoved", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnInputRemoved(FCadenceSequencerTrackInstanceInput Input);

	UFUNCTION(Category="Sequencer", DisplayName="OnEndUpdateInputs", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnEndUpdateInputs();

	UFUNCTION(Category="Sequencer", DisplayName="OnDestroyed", BlueprintImplementableEvent, meta=(CallInEditor=true))
	void K2_OnDestroyed();

public:

	UFUNCTION(Category="Sequencer", BlueprintCallable)
	TArray<FCadenceSequencerTrackInstanceInput> GetInputs() const;

	UFUNCTION(Category="Sequencer", BlueprintCallable)
	int32 GetNumInputs() const;

	UFUNCTION(Category="Sequencer", BlueprintCallable, BlueprintPure=false)
	FCadenceSequencerTrackInstanceInput GetInput(int32 Index) const;

private:

	virtual void OnInitialize() override { K2_OnInitialize(); }

	virtual void OnAnimate() override { K2_OnUpdate(); }

	virtual void OnBeginUpdateInputs() override { K2_OnBeginUpdateInputs(); }

	virtual void OnInputAdded(const FMovieSceneTrackInstanceInput& InInput) override;

	virtual void OnInputRemoved(const FMovieSceneTrackInstanceInput& InInput) override;

	virtual void OnEndUpdateInputs() override { K2_OnEndUpdateInputs(); }

	virtual void OnDestroyed() override;
};

