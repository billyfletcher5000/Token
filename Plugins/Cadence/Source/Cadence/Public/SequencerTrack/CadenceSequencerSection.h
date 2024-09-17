// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "EntitySystem/IMovieSceneEntityProvider.h"
#include "UObject/Object.h"
#include "CadenceSequencerSection.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CADENCE_API UCadenceSequencerSection : public UMovieSceneSection, public IMovieSceneEntityProvider
{
	GENERATED_BODY()

public:
	
	UCadenceSequencerSection(const FObjectInitializer& ObjInit);

protected:
	
	virtual void ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;
};
