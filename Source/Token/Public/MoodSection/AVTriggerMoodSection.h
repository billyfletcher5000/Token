// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "AVTrigger.h"
#include "UObject/Object.h"
#include "AVTriggerMoodSection.generated.h"

Expose_TNameOf(UAVTriggerMoodSectionDataAsset);

UCLASS(Blueprintable, BlueprintType)
class TOKEN_API UAVTriggerMoodSectionDataAsset : public UAVTriggerDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	FGuid SectionGUID;
};

class TOKEN_API UAVTriggerMoodSectionInstance : public UAVTriggerInstance
{
public:
	static UAVTriggerMoodSectionInstance* Create();
	
	virtual void BeginTracking() override;

private:
	UFUNCTION()
	void OnMoodSectionHit(const FGuid& MoodTrackSectionGUID);
};