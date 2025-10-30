// Copyright (C) Billy Fletcher 2025

#include "Reaction/CadenceReactionGroupAssetAction.h"

#include "Reaction/CadenceReactionGroup.h"

const FText FCadenceReactionGroupAssetAction::AssetName = FText::FromString(TEXT("Cadence Reaction Group"));
const FColor FCadenceReactionGroupAssetAction::TypeColor = FColor::Purple;

FCadenceReactionGroupAssetAction::FCadenceReactionGroupAssetAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText FCadenceReactionGroupAssetAction::GetName() const
{
	return AssetName;
}

FColor FCadenceReactionGroupAssetAction::GetTypeColor() const
{
	return TypeColor;
}

UClass* FCadenceReactionGroupAssetAction::GetSupportedClass() const
{
	return UCadenceReactionGroup::StaticClass();
}
/*
void FCadenceReactionGroupAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UCadenceReactionGroup* ReactionGroup = Cast<UCadenceReactionGroup>(Object);
		if(ReactionGroup != nullptr)
		{
		}
	}
}*/

uint32 FCadenceReactionGroupAssetAction::GetCategories()
{
	return AssetCategory;
}
