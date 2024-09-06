// Fill out your copyright notice in the Description page of Project Settings.

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
