// Fill out your copyright notice in the Description page of Project Settings.


#include "Transitions/CadenceTransitionTransform.h"

#include "Places/CadencePlaces.h"
#include "TickableActions/CadenceMoveTickableActions.h"

TArray<TScriptInterface<ICadenceTickableAction>> UCadenceTransitionTransform::CreateTickables(AActor* InActor, const float& InDuration, const FCadencePlace& InEndPlace)
{
	TArray<TScriptInterface<ICadenceTickableAction>> Tickables;

	if(InEndPlace.bIncludeLocation)
	{
		FVector StartLocation = InActor->GetActorLocation();
		UCadenceActorTranslateTickable* Tickable = UCadenceActorTranslateTickable::Create(InActor, InDuration, StartLocation, InEndPlace.Location, Ease);
		Tickables.Add(Tickable);
	}
	
	if(InEndPlace.bIncludeRotation)
	{
		FRotator StartRotation = InActor->GetActorRotation();
		UCadenceActorRotateTickable* Tickable = UCadenceActorRotateTickable::Create(InActor, InDuration, StartRotation, InEndPlace.Rotation, Ease, bRotationIsShortestPath);
		Tickables.Add(Tickable);
	}

	if(InEndPlace.bIncludeScale)
	{
		FVector StartScale = InActor->GetActorScale3D();
		UCadenceActorScaleTickable* Tickable = UCadenceActorScaleTickable::Create(InActor, InDuration, StartScale, InEndPlace.Scale, Ease);
		Tickables.Add(Tickable);		
	}

	return Tickables;
}

TArray<TScriptInterface<ICadenceTickableAction>> UCadenceTransitionTransformStepped::CreateTickables(AActor* InActor,
	const float& InDuration, const FCadencePlace& InEndPlace)
{
	TArray<TScriptInterface<ICadenceTickableAction>> Tickables;

	if(InEndPlace.bIncludeLocation)
	{
		FVector StartLocation = InActor->GetActorLocation();
		UCadenceActorTranslateSteppedTickable* Tickable = UCadenceActorTranslateSteppedTickable::Create(InActor, InDuration, StartLocation, InEndPlace.Location, StepProgressQuantization, StepDelayQuantization, Ease);
		Tickables.Add(Tickable);
	}

	if(InEndPlace.bIncludeRotation)
	{
		FRotator StartRotation = InActor->GetActorRotation();
		UCadenceActorRotateSteppedTickable* Tickable = UCadenceActorRotateSteppedTickable::Create(InActor, InDuration, StartRotation, InEndPlace.Rotation, StepProgressQuantization, StepDelayQuantization, Ease, bRotationIsShortestPath);
		Tickables.Add(Tickable);
	}
	
	if(InEndPlace.bIncludeScale)
	{
		FVector StartScale = InActor->GetActorScale3D();
		UCadenceActorScaleSteppedTickable* Tickable = UCadenceActorScaleSteppedTickable::Create(InActor, InDuration, StartScale, InEndPlace.Scale, StepProgressQuantization, StepDelayQuantization, Ease);
		Tickables.Add(Tickable);
	}

	return Tickables;
}
