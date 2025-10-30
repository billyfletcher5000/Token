// Copyright (C) Billy Fletcher 2025


#include "CadenceBlueprintEditorUtilities.h"

#include <rapidjson/reader.h>

#include "ContentBrowserModule.h"
#include "EditorDirectories.h"
#include "IContentBrowserSingleton.h"
#include "KismetCompilerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace FComponentCopyHelper
{
	static void CreateSceneComponentCopies(TArray<UActorComponent*>& ComponentList, UBlueprint* InBlueprint, USceneComponent* InComponent, USceneComponent* InParentComponent)
	{		
		USceneComponent* ComponentCopy = DuplicateObject<USceneComponent>(InComponent, InBlueprint, InComponent->GetFName());
		if(InParentComponent)
			ComponentCopy->SetupAttachment(InParentComponent);
		
		ComponentList.Add(ComponentCopy);
		
		TArray<USceneComponent*> Children;
		InComponent->GetChildrenComponents(false, Children);
		for(USceneComponent* ChildComponent : Children)
		{
			CreateSceneComponentCopies(ComponentList, InBlueprint, ChildComponent, ComponentCopy);
		}
	}
}

void UCadenceBlueprintEditorUtilities::CreateFrozenConstructionBlueprintCopy(AActor* InActor, UClass* InNewParentClass)
{
	FSaveAssetDialogConfig SaveAssetDialogConfig;
	SaveAssetDialogConfig.DialogTitleOverride = FText::FromString(TEXT("Save Asset As"));
	SaveAssetDialogConfig.DefaultPath = TEXT("/Game");
	SaveAssetDialogConfig.DefaultAssetName = TEXT("SimplifiedBlueprint");
	SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);

	if(SaveObjectPath.IsEmpty())
		return;

	const FString SavePackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
	const FString SavePackagePath = FPaths::GetPath(SavePackageName);
	const FString SaveAssetName = FPaths::GetBaseFilename(SavePackageName);	
	FEditorDirectories::Get().SetLastDirectory(ELastDirectory::NEW_ASSET, SavePackagePath);

	if(!InNewParentClass)
		InNewParentClass = AActor::StaticClass();

	if(!FKismetEditorUtilities::CanCreateBlueprintOfClass(InNewParentClass))
		return;

	UPackage* Package = CreatePackage(*SavePackageName);
	if(!Package)
		return;

	UClass* BpClass = nullptr;
	UClass* BpGenClass = nullptr;
	FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler").GetBlueprintTypesForClass(InNewParentClass, BpClass, BpGenClass);

	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(InNewParentClass, Package, *SaveAssetName, BPTYPE_Normal, BpClass, BpGenClass);

	TArray<UActorComponent*> CopiedComponents;
	auto Components = InActor->GetComponents();
	for(UActorComponent* Component : Components)
	{
		if(USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
		{
			if(SceneComponent != InActor->GetRootComponent())
				continue;

			FComponentCopyHelper::CreateSceneComponentCopies(CopiedComponents, Blueprint, SceneComponent, nullptr);
		}
		else
		{
			UActorComponent* ComponentCopy = DuplicateObject<UActorComponent>(Component, Blueprint, Component->GetFName());
			CopiedComponents.Add(ComponentCopy);
		}		
	}
	
	FKismetEditorUtilities::FAddComponentsToBlueprintParams AddCompParams;
	AddCompParams.HarvestMode = FKismetEditorUtilities::EAddComponentToBPHarvestMode::Harvest_UseComponentName;
	AddCompParams.bKeepMobility = true;
	FKismetEditorUtilities::AddComponentsToBlueprint(Blueprint, CopiedComponents, AddCompParams);

	FAssetRegistryModule::AssetCreated(Blueprint);
	// ReSharper disable once CppExpressionWithoutSideEffects
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	
	FKismetEditorUtilities::CompileBlueprint(Blueprint);
	
	UWorld* World = InActor->GetWorld();
	FActorSpawnParameters Params;
}