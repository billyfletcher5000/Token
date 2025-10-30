// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Factories/Factory.h"
#include "CustomDataAssetFactory.generated.h"

class UDataAsset;

DECLARE_LOG_CATEGORY_EXTERN(LogCustomDataAssetFactory, Log, All);

UCLASS()
class TOKENEDITOR_API UCustomDataAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=DataAsset)
	TSubclassOf<UDataAsset> DataAssetClass;

	// UFactory interface
	virtual bool ConfigureProperties() override { return true; }
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
