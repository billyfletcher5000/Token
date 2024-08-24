#include "CadenceGraphAssetAction.h"

#include "CadenceGraph.h"

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
	return UCadenceGraph::StaticClass();
}

void FCadenceGraphAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

uint32 FCadenceGraphAssetAction::GetCategories()
{
	return AssetCategory;
}
