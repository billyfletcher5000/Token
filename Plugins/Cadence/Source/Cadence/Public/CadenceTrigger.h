// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceTrigger.generated.h"

class UCadenceContext;

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceTriggerRunner : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Execute(UCadenceContext* InContext) PURE_VIRTUAL(UCadenceTriggerRunner::Execute, return true;);
	
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

protected:
	template<typename TRunner, typename TData>
	TRunner* TCreateRunner(TData* InData)
	{
		TRunner* Runner = NewObject<TRunner>();
		Runner->Init(InData);
		return Runner;
	}
};

class UQuantizedTimeTriggerData;

UCLASS()
class UQuantizedTimeTriggerRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	void Init(UQuantizedTimeTriggerData* InData);
	virtual bool Execute(UCadenceContext* InContext) override;

private:
	UPROPERTY()
	UQuantizedTimeTriggerData* Data;
};

UCLASS()
class CADENCE_API UQuantizedTimeTriggerData : public UCadenceTriggerData
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() override { return TCreateRunner<UQuantizedTimeTriggerRunner>(this);	}

public:
	UPROPERTY()
	EQuartzCommandQuantization TimePeriod;

	UPROPERTY()
	int32 Count = 1;
};

class USequenceTriggerData;

UCLASS()
class USequenceTriggerRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	void Init(USequenceTriggerData* InData);
	virtual bool Execute(UCadenceContext* InContext) override;

private:
	UPROPERTY()
	USequenceTriggerData* Data;
};

UCLASS()
class CADENCE_API USequenceTriggerData : public UCadenceTriggerData
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() override { return TCreateRunner<USequenceTriggerRunner>(this); }

public:
	UPROPERTY()
	EQuartzCommandQuantization TimePeriod;

	UPROPERTY()
	int32 Count = 1;
};