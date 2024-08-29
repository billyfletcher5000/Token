// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceTrigger.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceTriggerRunner : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, UCadenceTriggerRunner* /* Trigger */, InTrigger);
	FOnTriggered OnTriggeredDelegate;

protected:
	void Trigger();
};

UCLASS(Abstract)
class CADENCE_API UCadenceTriggerData : public UObject
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() PURE_VIRTUAL(UCadenceTriggerData::CreateRunner, return nullptr;);
};

UCLASS()
class CADENCE_API UQuantizedTimeTriggerData : public UCadenceTriggerData
{
	GENERATED_BODY()
};

UCLASS()
class UQuantizedTimeTriggerRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	
};
