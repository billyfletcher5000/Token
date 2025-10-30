// Copyright (C) Billy Fletcher 2025


#include "CadenceSettings.h"

#include "Cadence.h"
#include "Actors/CadenceActorSettings.h"

UCadenceSettings::UCadenceSettings()
{
	DefaultActorSettings = FString(TEXT("/Cadence/Default/Settings/DA_Cadence_DefaultActorSettings.DA_Cadence_DefaultActorSettings"));
}

UCadenceActorSettings* UCadenceSettings::GetDefaultActorSettings() const
{
	UCadenceActorSettings* ActorSettings = Cast<UCadenceActorSettings>(DefaultActorSettings.TryLoad());
	
	if(!ActorSettings)
	{
		UE_LOG(LogCadence, Warning, TEXT("No valid default actor settings set!"));
	}

	return ActorSettings;
}
