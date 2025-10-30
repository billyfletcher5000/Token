// Copyright (C) Billy Fletcher 2025


#include "CadenceBlueprintUtilities.h"

#include <rapidjson/reader.h>

#include "ContentBrowserModule.h"
#include "EditorDirectories.h"
#include "IContentBrowserSingleton.h"

AActor* UCadenceBlueprintUtilities::DuplicateActorComponentCopy(AActor* InActor, UClass* InNewActorClass)
{
	FSaveAssetDialogConfig SaveAssetDialogConfig;
	SaveAssetDialogConfig.DialogTitleOverride = FText::FromString(TEXT("Save Asset As"));
	SaveAssetDialogConfig.DefaultPath = TEXT("/Game");
	SaveAssetDialogConfig.DefaultAssetName = TEXT("SimplifiedBlueprint");
	SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);

	if(SaveObjectPath.IsEmpty())
		return nullptr;

	const FString SavePackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
	const FString SavePackagePath = FPaths::GetPath(SavePackageName);
	const FString SaveAssetName = FPaths::GetBaseFilename(SavePackageName);
	//FEditorDirectories::Get().SetLastDirectory(ELastDirectory::NEW_ASSET, SavePackagePath);
	
	/*
	UWorld* World = InActor->GetWorld();
	FActorSpawnParameters Params;

	AActor* OutActor = World->SpawnActor(InNewActorClass, &InActor->GetTransform());

	auto Components = InActor->GetComponents();
	for(UActorComponent* Component : Components)
	{
		UActorComponent* ComponentCopy = DuplicateObject(Component, OutActor, Component->GetFName());
		OutActor->AddOwnedComponent(ComponentCopy);
	}

	//UBlueprint* BP;
	//BP->ComponentTemplates*/

	return nullptr;
}
