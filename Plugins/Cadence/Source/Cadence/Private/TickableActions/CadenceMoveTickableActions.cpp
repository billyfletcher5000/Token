// Fill out your copyright notice in the Description page of Project Settings.


#include "TickableActions/CadenceMoveTickableActions.h"


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
