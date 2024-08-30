// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceTrigger.h"

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
}

bool USequenceTriggerRunner::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}
