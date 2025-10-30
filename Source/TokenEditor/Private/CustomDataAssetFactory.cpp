// Copyright (C) Billy Fletcher 2025

#include "CustomDataAssetFactory.h"

UCustomDataAssetFactory::UCustomDataAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDataAsset::StaticClass();
}

UObject* UCustomDataAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (DataAssetClass != nullptr)
	{
		return NewObject<UDataAsset>(InParent, DataAssetClass, Name, Flags | RF_Transactional);
	}
	else
	{
		// if we have no data asset class, use the passed-in class instead
		check(Class->IsChildOf(UDataAsset::StaticClass()));
		return NewObject<UDataAsset>(InParent, Class, Name, Flags);
	}
}