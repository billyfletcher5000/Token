// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceMath.h"
#include "TickableActions/CadenceMoveTickableActions.h"
#include "UObject/Object.h"
#include "CadenceTransitionTransform.generated.h"

struct FCadenceActorPlaceSet;
class ICadenceTickableAction;

UCLASS()
class CADENCE_API UCadenceTransitionTransform : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual TArray<TScriptInterface<ICadenceTickableAction>> CreateTickables(UWorld* InWorld, const FCadenceActorPlaceSet& InEndPlaceSet, const float& InDuration);

protected:
	UPROPERTY(EditInstanceOnly)
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;

	UPROPERTY(EditInstanceOnly)
	bool bRotationIsShortestPath = true;
};

UCLASS()
class CADENCE_API UCadenceTransitionTransformStepped : public UCadenceTransitionTransform
{
	GENERATED_BODY()

public:
	virtual TArray<TScriptInterface<ICadenceTickableAction>> CreateTickables(UWorld* InWorld, const FCadenceActorPlaceSet& InEndPlaceSet, const float& InDuration) override;

protected:
	UPROPERTY(EditInstanceOnly)
	FCadenceSteppedTickableParams Params;
};