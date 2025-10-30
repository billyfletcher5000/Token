// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"

#include "CadenceSettings.generated.h"

class UCadenceActorSettings;
/**
 * 
 */
UCLASS(config=Engine, defaultconfig)
class CADENCE_API UCadenceSettings : public UObject
{
	GENERATED_BODY()

public:
	UCadenceSettings();

	UCadenceActorSettings* GetDefaultActorSettings() const;

protected:
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Actors", meta = (AllowedClasses = "/Script/Cadence.CadenceActorSettings"))
	FSoftObjectPath DefaultActorSettings;
};
