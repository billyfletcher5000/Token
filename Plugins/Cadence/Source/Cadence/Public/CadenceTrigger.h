// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceTrigger.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceTrigger : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, UCadenceTrigger* /* Trigger */, InTrigger);
	FOnTriggered OnTriggeredDelegate;

protected:
	void Trigger();
};

UCLASS(Abstract)
class CADENCE_API UCadenceTriggerData : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class UQuantizedTimeTrigger : public UCadenceTrigger
{
	GENERATED_BODY()

public:
	
};
