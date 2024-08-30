// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceSubsystem.generated.h"

class UCadenceAsset;
class UCadenceGraphRunner;

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	// Begin UTickableWorldSubsystem
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// End UTickableWorldSubsystem

	UFUNCTION(BlueprintCallable)
	UCadenceGraphRunner* ActivateGraph(UCadenceAsset* CadenceAsset);

protected:
	friend UCadenceGraphRunner;

	void NotifyGraphComplete(UCadenceGraphRunner* InRunner);

private:
	UPROPERTY()
	TArray<UCadenceGraphRunner*> ActiveRunners;

	UPROPERTY()
	TArray<UCadenceGraphRunner*> EndedRunners;
};
