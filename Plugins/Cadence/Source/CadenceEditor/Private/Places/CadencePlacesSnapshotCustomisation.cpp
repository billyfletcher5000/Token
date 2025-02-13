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
			FCadencePlacePair* Pair = InSnapshot->Places.FindByPredicate([&ActorID] (const FCadencePlacePair& InPair)
			{
				return InPair.TrackedActorID == ActorID;
			});
			
			if(!Pair)
				continue;

			auto& Place = Pair->Place;

			if(Place.bIncludeLocation)
				ActorIter->SetActorLocation(Place.Location);

			if(Place.bIncludeRotation)
				ActorIter->SetActorRotation(Place.Rotation);

			if(Place.bIncludeScale)
				ActorIter->SetActorScale3D(Place.Scale);
		}
	}
	
	return FReply::Handled();
}
