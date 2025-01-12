// Copyright Epic Games, Inc. All Rights Reserved.

#include "CadenceEditorModule.h"

#include "CadenceSettings.h"
#include "Graph/CadenceGraph.h"
#include "CadenceGraphAssetAction.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphEditorPin.h"
#include "CadenceGraphPropertyCustomization.h"
#include "Reaction/CadenceReactionGroupAssetAction.h"
#include "CadenceSequencerSectionNameCustomization.h"
#include "CadenceSequencerTrackEditor.h"
#include "CadenceVariableInlineWidgetFunctions.h"
#include "IAssetTools.h"
#include "ISequencerModule.h"
#include "ISettingsModule.h"
#include "Reaction/CadenceReactionGroup.h"
#include "Interfaces/IPluginManager.h"
#include "Reaction/CadenceReactionGroupCustomization.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FCadenceEditorModule"

const FName FCadenceEditorModule::CategoryKey = FName(TEXT("Cadence"));
const FText FCadenceEditorModule::CategoryDisplayName = FText::FromString(TEXT("Cadence"));
const FName FCadenceEditorModule::StyleSetName = FName(TEXT("CadenceStyle"));

void FCadenceEditorModule::StartupModule()
{
	ISettingsModule* SettingsModule = FModuleManager::Get().GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Cadence", NSLOCTEXT("Cadence", "Cadence", "Cadence"),
			NSLOCTEXT("Cadence", "Configure Cadence settings", "Configure Cadence settings"), GetMutableDefault<UCadenceSettings>());
	}

	
	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);

	// Add Graph
	TSharedPtr<FCadenceGraphAssetAction> GraphAssetAction = MakeShareable(new FCadenceGraphAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(GraphAssetAction.ToSharedRef());

	TSharedPtr<FCadenceReactionGroupAssetAction> ReactionGroupAssetAction = MakeShareable(new FCadenceReactionGroupAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(ReactionGroupAssetAction.ToSharedRef());

	// Create Style Set
	StyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("Cadence");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	// Add Graph Icons
	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_Thumbnail_64x"), TEXT(".png")), FVector2D(64.0, 64.0));
	FSlateImageBrush* IconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_Icon_64x"), TEXT(".png")), FVector2D(64.0, 64.0));
	FSlateImageBrush* TrackIconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/Cadence_Icon_16x"), TEXT(".png")), FVector2D(16.0, 16.0));
	FSlateImageBrush* GridPreviewBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_GridPreview_Grid_128x"), TEXT(".png")), FVector2D(128.0, 128.0));
	FSlateImageBrush* GridPointBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icons/CadenceGraph_GridPreview_Point_11x"), TEXT(".png")), FVector2D(11.0, 11.0));
	
	StyleSet->Set(TEXT("ClassThumbnail.CadenceGraph"), ThumbnailBrush);
	StyleSet->Set(TEXT("ClassIcon.CadenceGraph"), IconBrush);
	StyleSet->Set(TEXT("ClassIcon.CadenceTrack"), TrackIconBrush);
	StyleSet->Set(TEXT("NodePreview.CadenceGrid"), GridPreviewBrush);
	StyleSet->Set(TEXT("NodePreview.GridPoint"), GridPointBrush);

	// Register Style Set
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	PanelPinFactory = MakeShareable(new FCadenceGraphEditorPanelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PanelPinFactory);
	
	NodeFactory = MakeShareable(new FCadenceGraphEditorNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(UCadenceGraph::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic( &FCadenceGraphCustomization::MakeInstance ) );
	PropertyModule.RegisterCustomClassLayout(UCadenceReactionGroup::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic( &FCadenceReactionGroupCustomization::MakeInstance));
	
	PropertyModule.NotifyCustomizationModuleChanged();

	FCadenceVariableInlineWidgetFunctions::RegisterAll(VariableToInlineWidgetFunc);
	
	ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
	CustomTrackEditorHandle = SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FCadenceSequencerTrackEditor::CreateTrackEditor));
}

void FCadenceEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Unregister Style Set
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);

	FEdGraphUtilities::UnregisterVisualPinFactory(PanelPinFactory);
	PanelPinFactory = nullptr;
	
	FEdGraphUtilities::UnregisterVisualNodeFactory(NodeFactory);
	NodeFactory = nullptr;
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.UnregisterCustomClassLayout(UCadenceGraph::StaticClass()->GetFName());
	PropertyModule.UnregisterCustomClassLayout(UCadenceReactionGroup::StaticClass()->GetFName());
	
	PropertyModule.UnregisterCustomPropertyTypeLayout(FCadenceGraphUserVariableSet::StaticStruct()->GetFName());
	PropertyModule.UnregisterCustomPropertyTypeLayout(FCadenceNamedVariable::StaticStruct()->GetFName());
	
	VariableToInlineWidgetFunc.Empty();
	
	if (ISequencerModule* SequencerModule = FModuleManager::Get().GetModulePtr<ISequencerModule>("Sequencer"))
	{
		SequencerModule->UnRegisterTrackEditor(CustomTrackEditorHandle);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCadenceEditorModule, Cadence)