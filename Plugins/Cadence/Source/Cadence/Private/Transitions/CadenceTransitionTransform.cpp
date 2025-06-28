// Fill out your copyright notice in the Description page of Project Settings.


#include "Transitions/CadenceTransitionTransform.h"

#include "CadenceSubsystem.h"
#include "Actors/CadenceTrackedActorComponent.h"
#include "Places/CadencePlaces.h"
#include "TickableActions/CadenceMoveTickableActions.h"

TArray<TScriptInterface<ICadenceTickableAction>> UCadenceTransitionTransform::CreateTickables(UWorld* InWorld, const FCadenceActorPlaceSet& InEndPlaceSet, const float& InDuration)
{	
	UCadenceSubsystem* CadenceSubsystem = InWorld->GetSubsystem<UCadenceSubsystem>();
	if(!IsValid(CadenceSubsystem))
		return {};
	
	AActor* Actor = CadenceSubsystem->GetTrackedActor(InEndPlaceSet.TrackedActorID);
	if(!IsValid(Actor))
		return {};
	
	TArray<TScriptInterface<ICadenceTickableAction>> Tickables;

	USceneComponent* RootSceneComponent = Actor->GetRootComponent();
	
	if(InEndPlaceSet.Place.bIncludeLocation)
	{
		FVector StartLocation = RootSceneComponent->GetRelativeLocation();
		UCadenceSceneComponentTranslateTickable* Tickable = UCadenceSceneComponentTranslateTickable::Create(RootSceneComponent, InDuration, StartLocation, InEndPlaceSet.Place.Location, Ease);
		Tickables.Add(Tickable);
	}
	
	if(InEndPlaceSet.Place.bIncludeRotation)
	{
		FRotator StartRotation = RootSceneComponent->GetRelativeRotation();
		UCadenceSceneComponentRotateTickable* Tickable = UCadenceSceneComponentRotateTickable::Create(RootSceneComponent, InDuration, StartRotation, InEndPlaceSet.Place.Rotation, Ease, bRotationIsShortestPath);
		Tickables.Add(Tickable);
	}

	if(InEndPlaceSet.Place.bIncludeScale)
	{
		FVector StartScale = RootSceneComponent->GetRelativeScale3D();
		UCadenceSceneComponentScaleTickable* Tickable = UCadenceSceneComponentScaleTickable::Create(RootSceneComponent, InDuration, StartScale, InEndPlaceSet.Place.Scale, Ease);
		Tickables.Add(Tickable);		
	}

	for(const FCadenceComponentPlacePair& ComponentPair : InEndPlaceSet.ComponentPlacePairs)
	{
		auto Components = Actor->GetComponentsByTag(USceneComponent::StaticClass(), FCadenceTrackingConstants::TrackedSceneComponentTag);
		for (auto& Component : Components)
		{
			if(Component->GetPathName(Actor) == ComponentPair.ComponentPathName)
			{
				if(USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
				{
					if(ComponentPair.Place.bIncludeLocation)
					{
						FVector StartLocation = SceneComponent->GetRelativeLocation();
						UCadenceSceneComponentTranslateTickable* Tickable = UCadenceSceneComponentTranslateTickable::Create(SceneComponent, InDuration, StartLocation, ComponentPair.Place.Location, Ease);
						Tickables.Add(Tickable);
					}

					if(ComponentPair.Place.bIncludeRotation)
					{
						FRotator StartRotation = SceneComponent->GetRelativeRotation();
						UCadenceSceneComponentRotateTickable* Tickable = UCadenceSceneComponentRotateTickable::Create(SceneComponent, InDuration, StartRotation, ComponentPair.Place.Rotation, Ease, bRotationIsShortestPath);
						Tickables.Add(Tickable);
					}

					if(ComponentPair.Place.bIncludeScale)
					{
						FVector StartScale = SceneComponent->GetRelativeScale3D();
						UCadenceSceneComponentScaleTickable* Tickable = UCadenceSceneComponentScaleTickable::Create(SceneComponent, InDuration, StartScale, ComponentPair.Place.Scale, Ease);
						Tickables.Add(Tickable);		
					}
				}
			}
		}
	}

	return Tickables;
}

TArray<TScriptInterface<ICadenceTickableAction>> UCadenceTransitionTransformStepped::CreateTickables(UWorld* InWorld, const FCadenceActorPlaceSet& InEndPlaceSet, const float& InDuration)
{
	UCadenceSubsystem* CadenceSubsystem = InWorld->GetSubsystem<UCadenceSubsystem>();
	if(!IsValid(CadenceSubsystem))
		return {};
	
	AActor* Actor = CadenceSubsystem->GetTrackedActor(InEndPlaceSet.TrackedActorID);
	if(!IsValid(Actor))
		return {};
	
	TArray<TScriptInterface<ICadenceTickableAction>> Tickables;

	USceneComponent* RootSceneComponent = Actor->GetRootComponent();
	
	if(InEndPlaceSet.Place.bIncludeLocation)
	{
		FVector StartLocation = RootSceneComponent->GetRelativeLocation();
		UCadenceSceneComponentTranslateSteppedTickable* Tickable = UCadenceSceneComponentTranslateSteppedTickable::Create(RootSceneComponent, InDuration, StartLocation, InEndPlaceSet.Place.Location, Params, Ease);
		Tickables.Add(Tickable);
	}

	if(InEndPlaceSet.Place.bIncludeRotation)
	{
		FRotator StartRotation = RootSceneComponent->GetRelativeRotation();
		UCadenceSceneComponentRotateSteppedTickable* Tickable = UCadenceSceneComponentRotateSteppedTickable::Create(RootSceneComponent, InDuration, StartRotation, InEndPlaceSet.Place.Rotation, Params, Ease, bRotationIsShortestPath);
		Tickables.Add(Tickable);
	}
	
	if(InEndPlaceSet.Place.bIncludeScale)
	{
		FVector StartScale = RootSceneComponent->GetRelativeScale3D();
		UCadenceSceneComponentScaleSteppedTickable* Tickable = UCadenceSceneComponentScaleSteppedTickable::Create(RootSceneComponent, InDuration, StartScale, InEndPlaceSet.Place.Scale, Params, Ease);
		Tickables.Add(Tickable);
	}

	return Tickables;
}
