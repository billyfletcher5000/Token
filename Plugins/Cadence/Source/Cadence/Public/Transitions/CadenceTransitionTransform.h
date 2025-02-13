// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceMath.h"
#include "UObject/Object.h"
#include "CadenceTransitionTransform.generated.h"

struct FCadencePlace;
class ICadenceTickableAction;

UCLASS()
class CADENCE_API UCadenceTransitionTranslate : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual TArray<ICadenceTickableAction*> CreateTickables(AActor* InActor, const float& InDuration, const FCadencePlace& InStartPlace, const FCadencePlace& InEndPlace);

protected:
	UPROPERTY(EditInstanceOnly)
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;

	UPROPERTY(EditInstanceOnly)
	bool bRotationIsShortestPath = true;
};

UCLASS()
class CADENCE_API UCadenceTransitionTranslateStepped : public UCadenceTransitionTranslate
{
	GENERATED_BODY()

public:
	virtual TArray<ICadenceTickableAction*> CreateTickables(AActor* InActor, const float& InDuration, const FCadencePlace& InStartPlace, const FCadencePlace& InEndPlace) override;

protected:
	UPROPERTY(EditInstanceOnly)
	EQuartzCommandQuantization StepProgressQuantization = EQuartzCommandQuantization::Beat;
	
	UPROPERTY(EditInstanceOnly)
	EQuartzCommandQuantization StepDelayQuantization = EQuartzCommandQuantization::QuarterNote;
};