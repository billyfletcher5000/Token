// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICadenceTickableAction.h"
#include "CadenceInterpolator.h"

#include "CadenceMoveTickableActions.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceActorTranslateTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceActorTranslateTickable* Create(AActor* InActor, const float& InDuration, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	static UCadenceActorTranslateTickable* Create(AActor* InActor, const float& InDuration, const FVector& InStartPoint, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> Actor;

	UPROPERTY()
	TObjectPtr<UCadenceInterpolatorVector> Interpolator;
};

UCLASS()
class CADENCE_API UCadenceActorRotateTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceActorRotateTickable* Create(AActor* InActor, const float& InDuration, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);
	static UCadenceActorRotateTickable* Create(AActor* InActor, const float& InDuration, const FRotator& InStartRotation, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> Actor;

	UPROPERTY()
	TObjectPtr<UCadenceInterpolatorRotator> Interpolator;
};
