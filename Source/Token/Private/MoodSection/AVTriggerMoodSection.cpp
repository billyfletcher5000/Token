// Copyright (C) Billy Fletcher 2025


#include "MoodSection/AVTriggerMoodSection.h"

#include "AudioVisualSubsystem.h"


UAVTriggerMoodSectionInstance* UAVTriggerMoodSectionInstance::Create()
{
	return NewObject<UAVTriggerMoodSectionInstance>();
}

void UAVTriggerMoodSectionInstance::BeginTracking()
{	
	UAudioVisualSubsystem* AVSubsystem = GWorld->GetSubsystem<UAudioVisualSubsystem>();
	if(ensure(AVSubsystem))
	{
		AVSubsystem->OnMoodTrackSectionHit.AddUniqueDynamic(this, &UAVTriggerMoodSectionInstance::OnMoodSectionHit);
	}
}

void UAVTriggerMoodSectionInstance::OnMoodSectionHit(const FGuid& MoodTrackSectionGUID)
{
	UAVTriggerMoodSectionDataAsset* MoodData = GetData<UAVTriggerMoodSectionDataAsset>();
	if(MoodTrackSectionGUID == MoodData->SectionGUID)
		Trigger();
}
