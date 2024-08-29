// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceTrigger.h"

void UCadenceTriggerRunner::Trigger()
{
	OnTriggeredDelegate.Broadcast(this);
}
