// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "TickableActions/ICadenceTickableAction.h"

#include "CadenceTrigger.generated.h"

class UCadenceAssetInstance;
class UCadenceContext;

/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceTriggerRunner : public UObject, public ICadenceTickableAction
{
	GENERATED_BODY()
	
public:	
	virtual bool Tick(const float& InDeltaSeconds) override PURE_VIRTUAL(UCadenceTriggerRunner::Tick, return false;);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, UCadenceTriggerRunner*, InTrigger);
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

class UCadenceTriggerQuantizedTimeData;

UCLASS()
class UCadenceTriggerQuantizedTimeRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	static UCadenceTriggerQuantizedTimeRunner* Create(UCadenceTriggerQuantizedTimeData* InData);
	virtual bool Tick(const float& InDeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<UCadenceTriggerQuantizedTimeData> Data;
};

UCLASS()
class CADENCE_API UCadenceTriggerQuantizedTimeData : public UCadenceTriggerData
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() override { return UCadenceTriggerQuantizedTimeRunner::Create(this);	}
	
public:
	UPROPERTY()
	EQuartzCommandQuantization TimePeriod;

	UPROPERTY()
	int32 Count = 1;
};

class UCadenceTriggerSequenceData;

UCLASS()
class UCadenceTriggerSequenceRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	static UCadenceTriggerSequenceRunner* Create(UCadenceTriggerSequenceData* InData);
	virtual bool Tick(const float& InDeltaSeconds) override;

private:
	UFUNCTION()
	void OnCurrentTriggerTriggered(UCadenceTriggerRunner* InTrigger);
	
private:
	UPROPERTY()
	UCadenceTriggerSequenceData* Data;

	UPROPERTY()
	int32 TriggerIndex = 0;

	UPROPERTY()
	UCadenceTriggerRunner* CurrentRunner = nullptr;

	UPROPERTY()
	int32 ActuatedCount = 0;
};

UCLASS()
class CADENCE_API UCadenceTriggerSequenceData : public UCadenceTriggerData
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() override { return UCadenceTriggerSequenceRunner::Create(this); }

public:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceTriggerData>> TriggerList;

	UPROPERTY()
	int32 Count = 1;
};

class UCadenceTriggerSequenceSectionData;

UCLASS()
class UCadenceTriggerSequenceSectionRunner : public UCadenceTriggerRunner
{
	GENERATED_BODY()

public:
	static UCadenceTriggerSequenceSectionRunner* Create(UCadenceTriggerSequenceSectionData* InData);
	virtual void Init() override;
	virtual bool Tick(const float& InDeltaSeconds) override;

private:
	UFUNCTION()
	void OnSectionPhaseComplete(FString InSectionName);

private:
	UPROPERTY()
	TObjectPtr<UCadenceTriggerSequenceSectionData> Data;

	UPROPERTY()
	bool bIsConditionSatisfied = false;
};

UENUM()
enum class ECadenceTriggerSequenceSectionPhase : uint8
{
	Start = 0,
	End
};	

UCLASS()
class CADENCE_API UCadenceTriggerSequenceSectionData : public UCadenceTriggerData
{
	GENERATED_BODY()

public:
	virtual UCadenceTriggerRunner* CreateRunner() override { return UCadenceTriggerSequenceSectionRunner::Create(this);	}
	
public:
	UPROPERTY()
	FCadenceSectionName SectionName;

	UPROPERTY()
	ECadenceTriggerSequenceSectionPhase Phase;

	UPROPERTY()
	int32 Count = 1;

	UPROPERTY()
	TWeakObjectPtr<UCadenceAssetInstance> AssetInstance;
};