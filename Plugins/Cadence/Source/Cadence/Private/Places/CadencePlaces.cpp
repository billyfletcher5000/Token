// Fill out your copyright notice in the Description page of Project Settings.


#include "Places/CadencePlaces.h"

#include "EngineUtils.h"
#include "Actors/CadenceTrackedActorComponent.h"

UCadencePlacesSnapshot* ACadenceStageActor::TakePlacesSnapshot()
{
	bool bIncludeAllChannels = TrackingChannels.Num() == 0;

	if(!bIncludeLocation && !bIncludeRotation && !bIncludeScale)
		return nullptr;

	UWorld* World = GetWorld();
	if(!IsValid(World))
		return nullptr;

	UCadencePlacesSnapshot* Snapshot = NewObject<UCadencePlacesSnapshot>();

	FBoxSphereBounds ThisBounds = GetBounds();
	Snapshot->StageSourceLocation = ThisBounds.Origin;
	
	for(TActorIterator<AActor> ActorIter(World); ActorIter; ++ActorIter)
	{
		if(UCadenceTrackedActorComponent* TrackedActorComponent = ActorIter->GetComponentByClass<UCadenceTrackedActorComponent>())
		{
			if(bIncludeAllChannels || TrackingChannels.Contains(TrackedActorComponent->GetTrackingChannel()))
			{
				if(!bUnlimitedBounds)
				{
					FVector ActorOrigin, ActorExtents;
					ActorIter->GetActorBounds(false, ActorOrigin, ActorExtents, true);

					FBox ActorBoundsBox = FBox::BuildAABB(ActorOrigin, ActorExtents);
					
					if(!ThisBounds.GetBox().Intersect(ActorBoundsBox))
						continue;
				}
				
				FCadenceActorPlaceSet PlaceSet;

				PlaceSet.TrackedActorID = TrackedActorComponent->GetTrackedActorID();
				
				PlaceSet.Place.bIncludeLocation = bIncludeLocation;
				PlaceSet.Place.bIncludeRotation = bIncludeRotation;
				PlaceSet.Place.bIncludeScale = bIncludeScale;

				if(bIncludeLocation)
					PlaceSet.Place.Location = ActorIter->GetRootComponent()->GetRelativeLocation();

				if(bIncludeRotation)
					PlaceSet.Place.Rotation = ActorIter->GetRootComponent()->GetRelativeRotation();

				if(bIncludeScale)
					PlaceSet.Place.Scale = ActorIter->GetRootComponent()->GetRelativeScale3D();

				auto Components = ActorIter->GetComponentsByTag(USceneComponent::StaticClass(), FCadenceTrackingConstants::TrackedSceneComponentTag);
				for(auto& Component : Components)
				{
					if(USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
					{
						FCadenceComponentPlacePair ComponentPair;

						ComponentPair.ComponentPathName = Component->GetPathName(*ActorIter);

						if(bIncludeLocation)
							ComponentPair.Place.Location = SceneComponent->GetRelativeLocation();

						if(bIncludeRotation)
							ComponentPair.Place.Rotation = SceneComponent->GetRelativeRotation();

						if(bIncludeScale)
							ComponentPair.Place.Scale = SceneComponent->GetRelativeScale3D();

						PlaceSet.ComponentPlacePairs.Add(ComponentPair);
					}
				}
				
				Snapshot->PlaceSets.Add(PlaceSet);
			}
		}
	}

	return Snapshot;
}
