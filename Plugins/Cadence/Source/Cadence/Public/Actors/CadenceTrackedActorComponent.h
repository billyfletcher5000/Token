// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CadenceTrackedActorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CADENCE_API UCadenceTrackedActorComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnComponentCreated() override;
	virtual void PostLoad() override;
	//virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGuid GetTrackedActorID() const { return TrackedActorID; }

	UFUNCTION(BlueprintCallable)
	void RegenerateCadenceGUID();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetTrackingChannel() const { return TrackingChannel; }

private:
	UPROPERTY(VisibleAnywhere)
	FGuid TrackedActorID;

	UPROPERTY(EditAnywhere)
	int32 TrackingChannel = 0;
};
