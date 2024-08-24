#include "CadenceGraphAssetAction.h"

#include "CadenceGraph.h"
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
	return UCadenceGraph::StaticClass();
}

void FCadenceGraphAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UCadenceGraph* CadenceGraph = Cast<UCadenceGraph>(Object);
		if(CadenceGraph != nullptr)
		{
			TSharedRef<FCadenceGraphApplication> EditorApplication(new FCadenceGraphApplication());
			EditorApplication->InitEditor(Mode, EditWithinLevelEditor, CadenceGraph);
		}
	}
}

uint32 FCadenceGraphAssetAction::GetCategories()
{
	return AssetCategory;
}
