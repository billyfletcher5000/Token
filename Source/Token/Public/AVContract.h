// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AVContract.generated.h"

class UAVTriggerDataAsset;
class UAVActorReferenceDataAsset;
class UAVEndStateDataAsset;
class UAVDurationSourceDataAsset;
class UAVTransitionDataAsset;
class UAVTransitionInstance;

/** Please add a class description */
UCLASS(Blueprintable, BlueprintType)
class TOKEN_API UAVContractDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVTriggerDataAsset> Trigger;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVActorReferenceDataAsset> TargetActor;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVEndStateDataAsset> EndState;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVDurationSourceDataAsset> Duration;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVTransitionDataAsset> Transition;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool DestroyActorOnComplete;	
};

UCLASS()
class TOKEN_API UAVContractInstance : public UObject
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void Init(UAVContractDataAsset* InDataAsset);
	
	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void Tick(double DeltaTime);

	UFUNCTION(BlueprintCallable)
	void Complete();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsStarted() const { return bIsStarted; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAVTransitionInstance* GetTransitionInstance() const { return TransitionInstance.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTargetActorInstance() const { return TargetActorInstance.Get(); }
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContractComplete, UAVContractInstance*, Contract);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="Events")
	FOnContractComplete OnContractComplete;
	
private:
	bool bIsStarted;
	TWeakObjectPtr<UAVContractDataAsset> Data;
	TWeakObjectPtr<UAVTransitionInstance> TransitionInstance;
	TWeakObjectPtr<AActor> TargetActorInstance;
};
