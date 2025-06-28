// Fill out your copyright notice in the Description page of Project Settings.

#include "SequencerTrack/CadenceSequencerTrackInstance.h"

#include "CadenceSubsystem.h"
#include "IMovieScenePlayer.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "SequencerTrack/CadenceSequencerSection.h"

TArray<FCadenceSequencerTrackInstanceInput> UCadenceSequencerTrackInstance::GetInputs() const
{
	using namespace UE::MovieScene;

	TArray<FCadenceSequencerTrackInstanceInput> Result;

	TArrayView<const FMovieSceneTrackInstanceInput> InputsView = Super::GetInputs();
	Result.Reserve(InputsView.Num());

	for (const FMovieSceneTrackInstanceInput& Input : InputsView)
	{
		FCadenceSequencerTrackInstanceInput ThisInput;
		ThisInput.Section = CastChecked<UCadenceSequencerSection>(Input.Section);

		const FInstanceRegistry* InstanceRegistry = GetLinker()->GetInstanceRegistry();
		if (ensure(InstanceRegistry->IsHandleValid(Input.InstanceHandle)))
		{
			ThisInput.Context = InstanceRegistry->GetInstance(Input.InstanceHandle).GetContext();
		}

		Result.Add(ThisInput);
	}

	return Result;
}

int32 UCadenceSequencerTrackInstance::GetNumInputs() const
{
	return GetInputs().Num();
}

FCadenceSequencerTrackInstanceInput UCadenceSequencerTrackInstance::GetInput(int32 Index) const
{
	using namespace UE::MovieScene;

	TArrayView<const FMovieSceneTrackInstanceInput> InputsView = Super::GetInputs();
	if (!InputsView.IsValidIndex(Index))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Attempting to acces input %d from an array of size %d."), Index, InputsView.Num()), ELogVerbosity::Error);
		return FCadenceSequencerTrackInstanceInput();
	}

	FMovieSceneTrackInstanceInput Input = InputsView[Index];

	FCadenceSequencerTrackInstanceInput Result;
	Result.Section = CastChecked<UCadenceSequencerSection>(Input.Section);

	const FInstanceRegistry* InstanceRegistry = GetLinker()->GetInstanceRegistry();
	if (ensure(InstanceRegistry->IsHandleValid(Input.InstanceHandle)))
	{
		Result.Context = InstanceRegistry->GetInstance(Input.InstanceHandle).GetContext();
	}

	return Result;
}

void UCadenceSequencerTrackInstance::OnInputAdded(const FMovieSceneTrackInstanceInput& InInput)
{
	using namespace UE::MovieScene;

	FCadenceSequencerTrackInstanceInput Result;
	Result.Section = CastChecked<UCadenceSequencerSection>(InInput.Section);

	const FInstanceRegistry* InstanceRegistry = GetLinker()->GetInstanceRegistry();
	if (ensure(InstanceRegistry->IsHandleValid(InInput.InstanceHandle)))
	{
		Result.Context = InstanceRegistry->GetInstance(InInput.InstanceHandle).GetContext();
	}

	if(GetWorld()->IsGameWorld())
	{
		UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
		if(ensure(Subsystem))
		{
			Subsystem->Notify_SectionStart(GetMovieSceneSequence(InstanceRegistry, InInput), Result.Section);
		}
	}

	K2_OnInputAdded(Result);
}

void UCadenceSequencerTrackInstance::OnInputRemoved(const FMovieSceneTrackInstanceInput& InInput)
{
	using namespace UE::MovieScene;

	FCadenceSequencerTrackInstanceInput Result;
	Result.Section = CastChecked<UCadenceSequencerSection>(InInput.Section);

	const FInstanceRegistry* InstanceRegistry = GetLinker()->GetInstanceRegistry();
	if (ensure(InstanceRegistry->IsHandleValid(InInput.InstanceHandle)))
	{
		Result.Context = InstanceRegistry->GetInstance(InInput.InstanceHandle).GetContext();		
	}

	if(GetWorld()->IsGameWorld())
	{
		UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
		if(ensure(Subsystem))
		{
			Subsystem->Notify_SectionEnd(GetMovieSceneSequence(InstanceRegistry, InInput), Result.Section);
		}
	}	

	K2_OnInputRemoved(Result);
}

void UCadenceSequencerTrackInstance::OnDestroyed()
{
	using namespace UE::MovieScene;

	TArrayView<const FMovieSceneTrackInstanceInput> InputsView = Super::GetInputs();
	for (const FMovieSceneTrackInstanceInput& Input : InputsView)
	{
		FCadenceSequencerTrackInstanceInput ThisInput;
		ThisInput.Section = CastChecked<UCadenceSequencerSection>(Input.Section);

		const FInstanceRegistry* InstanceRegistry = GetLinker()->GetInstanceRegistry();
		if (ensure(InstanceRegistry->IsHandleValid(Input.InstanceHandle)))
		{
			ThisInput.Context = InstanceRegistry->GetInstance(Input.InstanceHandle).GetContext();
		}

		if(GetWorld()->IsGameWorld())
		{
			UCadenceSubsystem* Subsystem = GetWorld()->GetSubsystem<UCadenceSubsystem>();
			if(Subsystem)
			{
				Subsystem->Notify_SectionEnd(GetMovieSceneSequence(InstanceRegistry, Input), ThisInput.Section);
			}
		}

		K2_OnInputRemoved(ThisInput);
	}
	K2_OnDestroyed();
}

UMovieSceneSequence* UCadenceSequencerTrackInstance::GetMovieSceneSequence(const UE::MovieScene::FInstanceRegistry* InstanceRegistry, const FMovieSceneTrackInstanceInput& InInput)
{
	using namespace UE::MovieScene;	
	
	const FSequenceInstance* RootSequenceInstance = nullptr;
	const FSequenceInstance& SequenceInstance = InstanceRegistry->GetInstance(InInput.InstanceHandle);
	if(!SequenceInstance.IsRootSequence())
	{
		RootSequenceInstance = &InstanceRegistry->GetInstance(SequenceInstance.GetRootInstanceHandle());
	}
	else
	{
		RootSequenceInstance = &SequenceInstance;
	}	
	
	FPlayerIndexPlaybackCapability* PlayerIndexPlaybackCapability = RootSequenceInstance->GetSharedPlaybackState()->FindCapability<FPlayerIndexPlaybackCapability>();
	IMovieScenePlayer* Player = PlayerIndexPlaybackCapability->GetPlayer(RootSequenceInstance->GetSharedPlaybackState());
	const FMovieSceneRootEvaluationTemplateInstance& EvalTemplate = Player->GetEvaluationTemplate();
	const FMovieSceneSequenceID SequenceID = RootSequenceInstance->GetSequenceID();
	return EvalTemplate.GetSequence(SequenceID);
}
