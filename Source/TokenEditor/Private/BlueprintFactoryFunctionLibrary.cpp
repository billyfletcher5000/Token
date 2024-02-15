// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFactoryFunctionLibrary.h"

#include "CustomBlueprintFactory.h"
#include "CustomDataAssetFactory.h"

UFactory* UBlueprintFactoryFunctionLibrary::CreateBlueprintFactory(const TSubclassOf<UObject> BPClass)
{
	UCustomBlueprintFactory* CustomBPFactory = NewObject<UCustomBlueprintFactory>();

	CustomBPFactory->SupportedClass = BPClass;
	CustomBPFactory->ParentClass = BPClass;

	return CustomBPFactory;
}

UFactory* UBlueprintFactoryFunctionLibrary::CreateDataAssetFactory(const TSubclassOf<UDataAsset> DataAssetClass)
{
	UCustomDataAssetFactory* CustomDataAssetFactory = NewObject<UCustomDataAssetFactory>();

	CustomDataAssetFactory->DataAssetClass = DataAssetClass;

	return CustomDataAssetFactory;
}
