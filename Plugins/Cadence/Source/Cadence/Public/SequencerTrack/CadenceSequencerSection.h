// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "EntitySystem/IMovieSceneEntityProvider.h"
#include "UObject/Object.h"
#include "CadenceSequencerSection.generated.h"

class UCadenceSequencerTrack;

UENUM(BlueprintType)
enum class ECadenceSectionEdgeQuantizationType : uint8
{
	Nearest = 0,
	Before,
	After,
	NoQuantization
};

USTRUCT(Blueprintable)
struct FCadenceSectionName
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Name = "Section";
};

/**
 * 
 */
UCLASS(Blueprintable)
class CADENCE_API UCadenceSequencerSection : public UMovieSceneSection, public IMovieSceneEntityProvider
{
	GENERATED_BODY()

public:
	
	UCadenceSequencerSection(const FObjectInitializer& ObjInit);

	FString GetSectionName() const { return SectionName.Name; }
	void SetSectionName(const FString& InName) { SectionName.Name = InName; }

	UCadenceSequencerTrack* GetParentTrack() const { return ParentTrack; }
	void SetParentTrack(UCadenceSequencerTrack* InParentTrack) { ParentTrack = InParentTrack; }

protected:
	
	virtual void ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;

public:
	UPROPERTY(EditAnywhere)
	FCadenceSectionName SectionName;

	UPROPERTY(EditAnywhere)
	ECadenceSectionEdgeQuantizationType StartEdgeQuantizationType = ECadenceSectionEdgeQuantizationType::Nearest;
	UPROPERTY(EditAnywhere)
	ECadenceSectionEdgeQuantizationType EndEdgeQuantizationType = ECadenceSectionEdgeQuantizationType::Nearest;
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization StartEdgeQuantizationBoundary = EQuartzCommandQuantization::Bar;
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization EndEdgeQuantizationBoundary = EQuartzCommandQuantization::Bar;
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceSequencerTrack> ParentTrack;
};
