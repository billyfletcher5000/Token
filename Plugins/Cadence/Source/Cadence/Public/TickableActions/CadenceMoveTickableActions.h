// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "ICadenceTickableAction.h"
#include "CadenceInterpolator.h"
#include "SequencerTrack/CadenceSequencerSection.h"

#include "CadenceMoveTickableActions.generated.h"

UCLASS()
class CADENCE_API UCadenceSceneComponentTranslateTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentTranslateTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	static UCadenceSceneComponentTranslateTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InStartPoint, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceInterpolatorVector> Interpolator;
};

USTRUCT()
struct FCadenceSteppedTickableParams
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	EQuartzCommandQuantization StepProgressQuantization = EQuartzCommandQuantization::Beat;
	
	UPROPERTY(EditInstanceOnly)
	EQuartzCommandQuantization StepDelayQuantization = EQuartzCommandQuantization::QuarterNote;

	UPROPERTY(EditInstanceOnly)
	bool bAlignFirstDelay = true;

	UPROPERTY(EditInstanceOnly)
	ECadenceSectionEdgeQuantizationType FirstDelayEdgeStyle = ECadenceSectionEdgeQuantizationType::After;

	UPROPERTY(EditInstanceOnly)
	EQuartzCommandQuantization FirstDelayNegativeTimeThresholdQuantization = EQuartzCommandQuantization::SixteenthNote;
};

UCLASS()
class CADENCE_API UCadenceSceneComponentTranslateSteppedTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentTranslateSteppedTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InEndPoint, const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	static UCadenceSceneComponentTranslateSteppedTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InStartPoint, const FVector& InEndPoint, const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceSteppedInterpolatorVector> Interpolator;
};

UCLASS()
class CADENCE_API UCadenceSceneComponentRotateTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentRotateTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);
	static UCadenceSceneComponentRotateTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FRotator& InStartRotation, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceInterpolatorRotator> Interpolator;
};

UCLASS()
class CADENCE_API UCadenceSceneComponentRotateSteppedTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentRotateSteppedTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FRotator& InEndRotation, const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);
	static UCadenceSceneComponentRotateSteppedTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FRotator& InStartRotation, const FRotator& InEndRotation, const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear, bool bInShortestPath = true);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceSteppedInterpolatorRotator> Interpolator;
};

UCLASS()
class CADENCE_API UCadenceSceneComponentScaleTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentScaleTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	static UCadenceSceneComponentScaleTickable* Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InStartScale, const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceInterpolatorVector> Interpolator;
};

UCLASS()
class CADENCE_API UCadenceSceneComponentScaleSteppedTickable : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:
	static UCadenceSceneComponentScaleSteppedTickable* Create(USceneComponent* InActor, const float& InDuration, const FVector& InEndScale,  const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);
	static UCadenceSceneComponentScaleSteppedTickable* Create(USceneComponent* InActor, const float& InDuration, const FVector& InStartScale, const FVector& InEndScale,  const FCadenceSteppedTickableParams& InSteppedParams, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase = ECadenceEasingFunc::Linear);

	virtual bool Tick(const float& InDeltaSeconds) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY()
	TObjectPtr<UCadenceSteppedInterpolatorVector> Interpolator;
};