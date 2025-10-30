// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceRhythmTrackMidiAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceRhythmTrackMidiAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UCadenceRhythmTrackMidiAssetFactory(const FObjectInitializer& ObjectInitializer);

public: // UFactory
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
};
