// Copyright (C) Billy Fletcher 2025


#include "AVTrigger.h"

void UAVTriggerInstance::Init(UAVTriggerDataAsset* InDataAsset)
{
	Data = InDataAsset;
}

void UAVTriggerInstance::Trigger()
{
	if(TriggerCount > Data->TriggerAmount)
		return;
	
	TriggerCount++;

	if(TriggerCount >= Data->TriggerAmount)
	{
		OnTriggered.Broadcast(this);

		if(Data->ResetCountOnTrigger)
			TriggerCount = 0;
	}
}
