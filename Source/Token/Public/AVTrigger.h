// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AVTrigger.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TOKEN_API UAVTriggerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	int32 TriggerAmount;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool ResetCountOnTrigger;
};

// Abstract
UCLASS(Abstract)
class TOKEN_API UAVTriggerInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(UAVTriggerDataAsset* InDataAsset);	
	virtual void Trigger();
	virtual void BeginTracking() {}
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrigger, UAVTriggerInstance*, TriggerInstance);
	FOnTrigger OnTriggered;

protected:
	template<typename T>
	T* GetData() { return dynamic_cast<T*>(Data.Get()); }
	
private:
	TWeakObjectPtr<UAVTriggerDataAsset> Data;
	
	int32 TriggerCount;
};
