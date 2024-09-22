// Fill out your copyright notice in the Description page of Project Settings.


#include "Triggers/CadenceTrigger.h"

#include "CadenceContext.h"

void UCadenceTriggerRunner::Trigger()
{
	OnTriggeredDelegate.Broadcast(this);
}

UCadenceTriggerQuantizedTimeRunner* UCadenceTriggerQuantizedTimeRunner::Create(UCadenceTriggerQuantizedTimeData* InData)
{
	UCadenceTriggerQuantizedTimeRunner* Runner = NewObject<UCadenceTriggerQuantizedTimeRunner>();
	Runner->Data = InData;
	return Runner;
}

bool UCadenceTriggerQuantizedTimeRunner::Tick(const float& InDeltaSeconds)
{
	return Super::Tick(InDeltaSeconds);
}

UCadenceTriggerSequenceRunner* UCadenceTriggerSequenceRunner::Create(UCadenceTriggerSequenceData* InData)
{
	UCadenceTriggerSequenceRunner* Runner = NewObject<UCadenceTriggerSequenceRunner>();
	Runner->Data = InData;
	return Runner;
}

bool UCadenceTriggerSequenceRunner::Tick(const float& InDeltaSeconds)
{
	int32 NumTriggers = Data->TriggerList.Num();
	
	if(NumTriggers == 0)
		return true;
	
	if(CurrentRunner == nullptr)
	{
		if(TriggerIndex >= NumTriggers)
		{
			ActuatedCount++;
			if(ActuatedCount >= Data->Count)
			{
				Trigger();
				return true;
			}
			
			TriggerIndex = 0;
		}
		
		CurrentRunner = Data->TriggerList[TriggerIndex]->CreateRunner();
		CurrentRunner->OnTriggeredDelegate.AddUniqueDynamic(this, &UCadenceTriggerSequenceRunner::OnCurrentTriggerTriggered);
	}
	
	return false;
}

void UCadenceTriggerSequenceRunner::OnCurrentTriggerTriggered(UCadenceTriggerRunner* InTrigger)
{
	InTrigger->OnTriggeredDelegate.RemoveDynamic(this, &UCadenceTriggerSequenceRunner::OnCurrentTriggerTriggered);
	TriggerIndex++;
}
