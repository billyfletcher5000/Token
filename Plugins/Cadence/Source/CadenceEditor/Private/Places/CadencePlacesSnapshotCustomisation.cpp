// Fill out your copyright notice in the Description page of Project Settings.


#include "Places/CadencePlacesSnapshotCustomisation.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EngineUtils.h"
#include "Actors/CadenceTrackedActorComponent.h"
#include "Places/CadencePlaces.h"

TSharedRef<IDetailCustomization> FCadencePlacesSnapshotDetailCustomization::MakeInstance()
{
	return MakeShareable(new FCadencePlacesSnapshotDetailCustomization);
}

void FCadencePlacesSnapshotDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto StageActors = DetailBuilder.GetSelectedObjectsOfType<UCadencePlacesSnapshot>();
	if(StageActors.Num() != 1)
		return;
	
	IDetailCategoryBuilder& LevelCat = DetailBuilder.EditCategory(TEXT("Level"));
	LevelCat.AddCustomRow(FText::FromString(TEXT("ApplySnapshot")))
	.ValueContent()
	[
		SNew(SButton)
		.Text(FText::FromString(TEXT("Apply Snapshot")))
		.OnClicked(this, &FCadencePlacesSnapshotDetailCustomization::OnApplySnapshot, StageActors[0])
	];	
}

FReply FCadencePlacesSnapshotDetailCustomization::OnApplySnapshot(TWeakObjectPtr<UCadencePlacesSnapshot> InSnapshot)
{
	FWorldContext& WorldContext = GEditor->GetEditorWorldContext();
	
	UWorld* World = WorldContext.World();
	if(!IsValid(World))
		return FReply::Unhandled();

	for(TActorIterator<AActor> ActorIter(World); ActorIter; ++ActorIter)
	{
		if(UCadenceTrackedActorComponent* TrackedActorComponent = ActorIter->GetComponentByClass<UCadenceTrackedActorComponent>())
		{
			FGuid ActorID = TrackedActorComponent->GetTrackedActorID();
			FCadenceActorPlaceSet* Pair = InSnapshot->PlaceSets.FindByPredicate([&ActorID] (const FCadenceActorPlaceSet& InPair)
			{
				return InPair.TrackedActorID == ActorID;
			});
			
			if(!Pair)
				continue;

			auto& Place = Pair->Place;

			if(Place.bIncludeLocation)
				ActorIter->GetRootComponent()->SetRelativeLocation(Place.Location);

			if(Place.bIncludeRotation)
				ActorIter->GetRootComponent()->SetRelativeRotation(Place.Rotation);

			if(Place.bIncludeScale)
				ActorIter->GetRootComponent()->SetRelativeScale3D(Place.Scale);

			for(FCadenceComponentPlacePair& ComponentPair : Pair->ComponentPlacePairs)
			{
				auto Components = ActorIter->GetComponentsByTag(USceneComponent::StaticClass(), FCadenceTrackingConstants::TrackedSceneComponentTag);
				for (auto& Component : Components)
				{
					if(Component->GetPathName(*ActorIter) == ComponentPair.ComponentPathName)
					{
						if(USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
						{
							if(ComponentPair.Place.bIncludeLocation)
								SceneComponent->SetRelativeLocation(ComponentPair.Place.Location);

							if(ComponentPair.Place.bIncludeRotation)
								SceneComponent->SetRelativeRotation(ComponentPair.Place.Rotation);

							if(ComponentPair.Place.bIncludeScale)
								SceneComponent->SetRelativeScale3D(ComponentPair.Place.Scale);
						}
					}
				}
			}
		}
	}
	
	return FReply::Handled();
}
