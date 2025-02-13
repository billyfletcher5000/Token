// Fill out your copyright notice in the Description page of Project Settings.


#include "Places/CadencePlaces.h"

#include "EngineUtils.h"
#include "Actors/CadenceTrackedActorComponent.h"

UCadencePlacesSnapshot* ACadenceStageActor::TakePlacesSnapshot()
{
	bool bIncludeAllChannels = TrackingChannels.Num() == 0;

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
				
				FCadencePlacePair PlacePair;

				PlacePair.TrackedActorID = TrackedActorComponent->GetTrackedActorID();
				
				PlacePair.Place.bIncludeLocation = bIncludeLocation;
				PlacePair.Place.bIncludeRotation = bIncludeRotation;
				PlacePair.Place.bIncludeScale = bIncludeScale;

				if(bIncludeLocation)
					PlacePair.Place.Location = ActorIter->GetActorLocation();

				if(bIncludeRotation)
					PlacePair.Place.Rotation = ActorIter->GetActorRotation();

				if(bIncludeScale)
					PlacePair.Place.Scale = ActorIter->GetActorScale3D();
				
				Snapshot->Places.Add(PlacePair);
			}
		}
	}

	return Snapshot;
}
