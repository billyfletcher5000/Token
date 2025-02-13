// Fill out your copyright notice in the Description page of Project Settings.


#include "Places/CadenceStageDetailCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IAssetTools.h"
#include "Places/CadencePlaces.h"

TSharedRef<IDetailCustomization> FCadenceStageDetailCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceStageDetailCustomization);
}

void FCadenceStageDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto StageActors = DetailBuilder.GetSelectedObjectsOfType<ACadenceStageActor>();
	if(StageActors.Num() != 1)
		return;
	
	IDetailCategoryBuilder& StageCat = DetailBuilder.EditCategory(TEXT("Stage"));
	StageCat.AddCustomRow(FText::FromString(TEXT("TakeSnapshot")))
	.ValueContent()
	[
		SNew(SButton)
		.Text(FText::FromString(TEXT("Take Snapshot")))
		.OnClicked(this, &FCadenceStageDetailCustomization::OnTakeSnapshot, StageActors[0])
	];	
}

FReply FCadenceStageDetailCustomization::OnTakeSnapshot(TWeakObjectPtr<ACadenceStageActor> InActor)
{
	if(!InActor.IsValid())
		return FReply::Unhandled();

	UCadencePlacesSnapshot* Snapshot = InActor->TakePlacesSnapshot();
	if(!IsValid(Snapshot))
		return FReply::Unhandled();

	FString PackageName;
	FString AssetName;
	IAssetTools::Get().CreateUniqueAssetName(TEXT("PlaceSnapshot"), TEXT(""), PackageName, AssetName);
	IAssetTools::Get().DuplicateAssetWithDialog(AssetName, PackageName, Snapshot);
	
	return FReply::Handled();
}
