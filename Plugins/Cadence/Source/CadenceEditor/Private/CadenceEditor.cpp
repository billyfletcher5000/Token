// Copyright Epic Games, Inc. All Rights Reserved.

#include "CadenceEditor.h"

#include "CadenceGraph.h"
#include "CadenceGraphAssetAction.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphEditorPin.h"
#include "CadenceGraphUserVariableSetCustomization.h"
#include "IAssetTools.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FCadenceEditorModule"

const FName FCadenceEditorModule::CategoryKey = FName(TEXT("Cadence"));
const FText FCadenceEditorModule::CategoryDisplayName = FText::FromString(TEXT("Cadence"));
const FName FCadenceEditorModule::StyleSetName = FName(TEXT("CadenceStyle"));

void FCadenceEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);

	// Add Graph
	TSharedPtr<FCadenceGraphAssetAction> GraphAssetAction = MakeShareable(new FCadenceGraphAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(GraphAssetAction.ToSharedRef());

	// Create Style Set
	StyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("Cadence");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	// Add Graph Icons
	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_Thumbnail_64x"), TEXT(".png")), FVector2D(64.0, 64.0));
	FSlateImageBrush* IconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_Icon_64x"), TEXT(".png")), FVector2D(64.0, 64.0));
	StyleSet->Set(TEXT("ClassThumbnail.CadenceGraph"), ThumbnailBrush);
	StyleSet->Set(TEXT("ClassIcon.CadenceGraph"), IconBrush);

	// Register Style Set
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	PanelPinFactory = MakeShareable(new FCadenceGraphEditorPanelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PanelPinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout(FCadenceGraphUserVariableSet::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FCadenceGraphUserVariableSetCustomization::MakeInstance ) );

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FCadenceEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Unregister Style Set
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);

	FEdGraphUtilities::UnregisterVisualPinFactory(PanelPinFactory);
	PanelPinFactory = nullptr;
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(FCadenceGraphUserVariableSet::StaticStruct()->GetFName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCadenceEditorModule, Cadence)