// Fill out your copyright notice in the Description page of Project Settings.


#include "TickableActions/CadenceMoveTickableActions.h"

#include "CadenceAssetInstance.h"
#include "CadenceSubsystem.h"


UCadenceActorTranslateTickable* UCadenceActorTranslateTickable::Create(AActor* InActor, const float& InDuration, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InActor, InDuration, InActor->GetActorLocation(), InEndPoint, InEase);	
}

UCadenceActorTranslateTickable* UCadenceActorTranslateTickable::Create(AActor* InActor, const float& InDuration, const FVector& InStartPoint, const FVector& InEndPoint, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	UCadenceActorTranslateTickable* Action = NewObject<UCadenceActorTranslateTickable>();
	Action->Actor = InActor;
	Action->Interpolator = UCadenceInterpolatorVector::Create(InStartPoint, InEndPoint, InDuration, InEase);
	return Action;
}

bool UCadenceActorTranslateTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorLocation(Interpolator->GetValue());
	return bResult;
}

UCadenceActorTranslateSteppedTickable* UCadenceActorTranslateSteppedTickable::Create(AActor* InActor, const float& InDuration,
	const FVector& InEndPoint, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InActor, InDuration, InActor->GetActorLocation(), InEndPoint, InStepProgressQuantization, InStepDelayQuantization, InEase);
}

UCadenceActorTranslateSteppedTickable* UCadenceActorTranslateSteppedTickable::Create(AActor* InActor, const float& InDuration,
	const FVector& InStartPoint, const FVector& InEndPoint,
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
				
				UCadenceActorTranslateSteppedTickable* Action = NewObject<UCadenceActorTranslateSteppedTickable>();
				Action->Actor = InActor;
				Action->Interpolator = UCadenceSteppedInterpolatorVector::Create(InStartPoint, InEndPoint, InDuration, StepProgressDuration, StepDelayDuration, InEase);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceActorTranslateSteppedTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorLocation(Interpolator->GetValue());
	return bResult;
}

UCadenceActorRotateTickable* UCadenceActorRotateTickable::Create(AActor* InActor, const float& InDuration,
                                                                 const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase, bool bInShortestPath)
{
	return Create(InActor, InDuration, InActor->GetActorRotation(), InEndRotation, InEase, bInShortestPath);
}

UCadenceActorRotateTickable* UCadenceActorRotateTickable::Create(AActor* InActor, const float& InDuration,
	 const FRotator& InStartRotation, const FRotator& InEndRotation, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase, bool bInShortestPath)
{	
	UCadenceActorRotateTickable* Action = NewObject<UCadenceActorRotateTickable>();
	Action->Actor = InActor;
	Action->Interpolator = UCadenceInterpolatorRotator::Create(InStartRotation, InEndRotation, InDuration, InEase, bInShortestPath);
	return Action;
}

bool UCadenceActorRotateTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorRotation(Interpolator->GetValue());
	return bResult;
}

UCadenceActorRotateSteppedTickable* UCadenceActorRotateSteppedTickable::Create(AActor* InActor, const float& InDuration,
	const FRotator& InEndRotation, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase,
	bool bInShortestPath)
{
	return Create(InActor, InDuration, InActor->GetActorRotation(), InEndRotation, InStepProgressQuantization, InStepDelayQuantization, InEase, bInShortestPath);
}

UCadenceActorRotateSteppedTickable* UCadenceActorRotateSteppedTickable::Create(AActor* InActor, const float& InDuration,
	const FRotator& InStartRotation, const FRotator& InEndRotation,
	const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase,
	bool bInShortestPath)
{
	if(const UWorld* World = InActor->GetWorld())
	{
		if(const UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>())
		{
			if(UCadenceAssetInstance* ActiveCadenceAsset = CadenceSubsystem->GetPrimaryActiveAsset())
			{
				float StepProgressDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepProgressQuantization);
				float StepDelayDuration = ActiveCadenceAsset->GetQuantizationDuration(InStepDelayQuantization);
				
				UCadenceActorRotateSteppedTickable* Action = NewObject<UCadenceActorRotateSteppedTickable>();
				Action->Actor = InActor;
				Action->Interpolator = UCadenceSteppedInterpolatorRotator::Create(InStartRotation, InEndRotation, InDuration, StepProgressDuration, StepDelayDuration, InEase, bInShortestPath);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceActorRotateSteppedTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorRotation(Interpolator->GetValue());
	return bResult;
}

UCadenceActorScaleTickable* UCadenceActorScaleTickable::Create(AActor* InActor, const float& InDuration,
	const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InActor, InDuration, InActor->GetActorScale3D(), InEndScale, InEase);
}

UCadenceActorScaleTickable* UCadenceActorScaleTickable::Create(AActor* InActor, const float& InDuration,
	const FVector& InStartScale, const FVector& InEndScale, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	UCadenceActorScaleTickable* Action = NewObject<UCadenceActorScaleTickable>();
	Action->Actor = InActor;
	Action->Interpolator = UCadenceInterpolatorVector::Create(InStartScale, InEndScale, InDuration, InEase);
	return Action;
}

bool UCadenceActorScaleTickable::Tick(const float& InDeltaSeconds)
{
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorScale3D(Interpolator->GetValue());
	return bResult;
}

UCadenceActorScaleSteppedTickable* UCadenceActorScaleSteppedTickable::Create(AActor* InActor, const float& InDuration,
	const FVector& InEndScale, const EQuartzCommandQuantization& InStepProgressQuantization,
	const EQuartzCommandQuantization& InStepDelayQuantization, const TEnumAsByte<ECadenceEasingFunc::Type>& InEase)
{
	return Create(InActor, InDuration, InActor->GetActorScale3D(), InEndScale, InStepProgressQuantization, InStepDelayQuantization, InEase);
}

UCadenceActorScaleSteppedTickable* UCadenceActorScaleSteppedTickable::Create(AActor* InActor, const float& InDuration,
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
				
				UCadenceActorScaleSteppedTickable* Action = NewObject<UCadenceActorScaleSteppedTickable>();
				Action->Actor = InActor;
				Action->Interpolator = UCadenceSteppedInterpolatorVector::Create(InStartScale, InEndScale, InDuration, StepProgressDuration, StepDelayDuration, InEase);
				return Action;
			}
		}
	}

	return nullptr;
}

bool UCadenceActorScaleSteppedTickable::Tick(const float& InDeltaSeconds)
{	
	bool bResult = Interpolator->Tick(InDeltaSeconds);	
	Actor->SetActorScale3D(Interpolator->GetValue());
	return bResult;
}
