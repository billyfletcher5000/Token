// Fill out your copyright notice in the Description page of Project Settings.


#include "Triggers/CadenceTrigger.h"

#include "CadenceContext.h"

void UCadenceTriggerRunner::Trigger()
{
	OnTriggeredDelegate.Broadcast(this);
}

void UQuantizedTimeTriggerRunner::Init(UQuantizedTimeTriggerData* InData)
{
	Data = InData;
}

bool UQuantizedTimeTriggerRunner::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

void USequenceTriggerRunner::Init(USequenceTriggerData* InData)
{
	Data = InData;
	CurrentRunner = nullptr;
	TriggerIndex = 0;
	ActuatedCount = 0;
}

bool USequenceTriggerRunner::Execute(UCadenceContext* InContext)
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
				return true;
			
			TriggerIndex = 0;
		}
		
		CurrentRunner = Data->TriggerList[TriggerIndex]->CreateRunner();
		CurrentRunner->OnTriggeredDelegate.AddUniqueDynamic(this, &USequenceTriggerRunner::OnCurrentTriggerTriggered);
	}
	
	return false;
}

void USequenceTriggerRunner::OnCurrentTriggerTriggered(UCadenceTriggerRunner* InTrigger)
{
	InTrigger->OnTriggeredDelegate.RemoveDynamic(this, &USequenceTriggerRunner::OnCurrentTriggerTriggered);
	TriggerIndex++;
}
