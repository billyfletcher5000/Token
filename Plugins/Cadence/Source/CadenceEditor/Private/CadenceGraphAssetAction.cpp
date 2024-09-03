#include "CadenceGraphAssetAction.h"

#include "CadenceAsset.h"
#include "Graph/CadenceGraph.h"
#include "CadenceGraphApplication.h"

const FText FCadenceGraphAssetAction::AssetName = FText::FromString(TEXT("Cadence Graph"));
const FColor FCadenceGraphAssetAction::TypeColor = FColor::Purple;

FCadenceGraphAssetAction::FCadenceGraphAssetAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText FCadenceGraphAssetAction::GetName() const
{
	return AssetName;
}

FColor FCadenceGraphAssetAction::GetTypeColor() const
{
	return TypeColor;
}

UClass* FCadenceGraphAssetAction::GetSupportedClass() const
{
	return UCadenceAsset::StaticClass();
}

void FCadenceGraphAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UCadenceAsset* CadenceAsset = Cast<UCadenceAsset>(Object);
		if(CadenceAsset != nullptr)
		{
			TSharedRef<FCadenceGraphApplication> EditorApplication(new FCadenceGraphApplication());
			EditorApplication->InitEditor(Mode, EditWithinLevelEditor, CadenceAsset);
		}
	}
}

uint32 FCadenceGraphAssetAction::GetCategories()
{
	return AssetCategory;
}
