// Fill out your copyright notice in the Description page of Project Settings.


#include "TickableActions/CadenceMoveTickableActions.h"

#include "CadenceAssetInstance.h"
#include "CadenceSubsystem.h"


UCadenceSceneComponentTranslateTickable* UCadenceSceneComponentTranslateTickable::Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InSceneComponent, InDuration, InSceneComponent->GetRelativeLocation(), InEndPoint, InEase);	
}

UCadenceSceneComponentTranslateTickable* UCadenceSceneComponentTranslateTickable::Create(USceneComponent* InSceneComponent, const float& InDuration, const FVector& InStartPoint, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	UCadenceSceneComponentTranslateTickable* Action = NewObject<UCadenceSceneComponentTranslateTickable>();
	Action->SceneComponent = InSceneComponent;
	Action->Interpolator = UCadenceInterpolatorVector::Create(InStartPoint, InEndPoint, InDuration, InEase);
	return Action;
}

bool UCadenceSceneComponentTranslateTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeLocation(Interpolator->GetValue());
	return bResult;
}

UCadenceSceneComponentTranslateSteppedTickable* UCadenceSceneComponentTranslateSteppedTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FVector& InEndPoint, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InSceneComponent, InDuration, InSceneComponent->GetRelativeLocation(), InEndPoint, InStepProgressQuantization, InStepDelayQuantization, InEase);
}

UCadenceSceneComponentTranslateSteppedTickable* UCadenceSceneComponentTranslateSteppedTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FVector& InStartPoint, const FVector& InEndPoint,
	const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	if(const UWorld* World = InSceneComponent->GetWorld())
	{
		if(const UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>())
		{
			if(UCadenceAssetInstance* ActiveCadenceAsset = CadenceSubsystem->GetPrimaryActiveAsset())
			{
				float StepProgressDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepProgressQuantization);
				float StepDelayDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepDelayQuantization);
				
				UCadenceSceneComponentTranslateSteppedTickable* Action = NewObject<UCadenceSceneComponentTranslateSteppedTickable>();
				Action->SceneComponent = InSceneComponent;
				Action->Interpolator = UCadenceSteppedInterpolatorVector::Create(InStartPoint, InEndPoint, InDuration, StepProgressDuration, StepDelayDuration, InEase);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceSceneComponentTranslateSteppedTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeLocation(Interpolator->GetValue());
	return bResult;
}

UCadenceSceneComponentRotateTickable* UCadenceSceneComponentRotateTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
                                                                 const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase, bool bInShortestPath)
{
	return Create(InSceneComponent, InDuration, InSceneComponent->GetRelativeRotation(), InEndRotation, InEase, bInShortestPath);
}

UCadenceSceneComponentRotateTickable* UCadenceSceneComponentRotateTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	 const FRotator& InStartRotation, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase, bool bInShortestPath)
{	
	UCadenceSceneComponentRotateTickable* Action = NewObject<UCadenceSceneComponentRotateTickable>();
	Action->SceneComponent = InSceneComponent;
	Action->Interpolator = UCadenceInterpolatorRotator::Create(InStartRotation, InEndRotation, InDuration, InEase, bInShortestPath);
	return Action;
}

bool UCadenceSceneComponentRotateTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeRotation(Interpolator->GetValue());
	return bResult;
}

UCadenceSceneComponentRotateSteppedTickable* UCadenceSceneComponentRotateSteppedTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FRotator& InEndRotation, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase,
	bool bInShortestPath)
{
	return Create(InSceneComponent, InDuration, InSceneComponent->GetRelativeRotation(), InEndRotation, InStepProgressQuantization, InStepDelayQuantization, InEase, bInShortestPath);
}

UCadenceSceneComponentRotateSteppedTickable* UCadenceSceneComponentRotateSteppedTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FRotator& InStartRotation, const FRotator& InEndRotation,
	const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase,
	bool bInShortestPath)
{
	if(const UWorld* World = InSceneComponent->GetWorld())
	{
		if(const UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>())
		{
			if(UCadenceAssetInstance* ActiveCadenceAsset = CadenceSubsystem->GetPrimaryActiveAsset())
			{
				float StepProgressDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepProgressQuantization);
				float StepDelayDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepDelayQuantization);
				
				UCadenceSceneComponentRotateSteppedTickable* Action = NewObject<UCadenceSceneComponentRotateSteppedTickable>();
				Action->SceneComponent = InSceneComponent;
				Action->Interpolator = UCadenceSteppedInterpolatorRotator::Create(InStartRotation, InEndRotation, InDuration, StepProgressDuration, StepDelayDuration, InEase, bInShortestPath);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceSceneComponentRotateSteppedTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeRotation(Interpolator->GetValue());
	return bResult;
}

UCadenceSceneComponentScaleTickable* UCadenceSceneComponentScaleTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InSceneComponent, InDuration, InSceneComponent->GetRelativeScale3D(), InEndScale, InEase);
}

UCadenceSceneComponentScaleTickable* UCadenceSceneComponentScaleTickable::Create(USceneComponent* InSceneComponent, const float& InDuration,
	const FVector& InStartScale, const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	UCadenceSceneComponentScaleTickable* Action = NewObject<UCadenceSceneComponentScaleTickable>();
	Action->SceneComponent = InSceneComponent;
	Action->Interpolator = UCadenceInterpolatorVector::Create(InStartScale, InEndScale, InDuration, InEase);
	return Action;
}

bool UCadenceSceneComponentScaleTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeScale3D(Interpolator->GetValue());
	return bResult;
}

UCadenceSceneComponentScaleSteppedTickable* UCadenceSceneComponentScaleSteppedTickable::Create(USceneComponent* InActor, const float& InDuration,
	const FVector& InEndScale, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InActor, InDuration, InActor->GetRelativeScale3D(), InEndScale, InStepProgressQuantization, InStepDelayQuantization, InEase);
}

UCadenceSceneComponentScaleSteppedTickable* UCadenceSceneComponentScaleSteppedTickable::Create(USceneComponent* InActor, const float& InDuration,
	const FVector& InStartScale, const FVector& InEndScale,
	const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	if(const UWorld* World = InActor->GetWorld())
	{
		if(const UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>())
		{
			if(UCadenceAssetInstance* ActiveCadenceAsset = CadenceSubsystem->GetPrimaryActiveAsset())
			{
				float StepProgressDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepProgressQuantization);
				float StepDelayDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepDelayQuantization);
				
				UCadenceSceneComponentScaleSteppedTickable* Action = NewObject<UCadenceSceneComponentScaleSteppedTickable>();
				Action->SceneComponent = InActor;
				Action->Interpolator = UCadenceSteppedInterpolatorVector::Create(InStartScale, InEndScale, InDuration, StepProgressDuration, StepDelayDuration, InEase);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceSceneComponentScaleSteppedTickable::Tick(const float& InDeltaSeconds)
{	
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	SceneComponent->SetRelativeScale3D(Interpolator->GetValue());
	return bResult;
}
