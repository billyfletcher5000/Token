// Copyright Epic Games, Inc. All Rights Reserved.

#include "CadenceEditor.h"

#include "CadenceGraphAssetAction.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FCadenceEditorModule"

const FName FCadenceEditorModule::CategoryKey = FName(TEXT("Cadence"));
const FText FCadenceEditorModule::CategoryDisplayName = FText::FromString(TEXT("Cadence"));

void FCadenceEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);

	// Add Graph
	TSharedPtr<FCadenceGraphAssetAction> GraphAssetAction = MakeShareable(new FCadenceGraphAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(GraphAssetAction.ToSharedRef());
}

void FCadenceEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCadenceEditorModule, Cadence)