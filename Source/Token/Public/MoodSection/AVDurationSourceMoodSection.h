// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AVDurationSource.h"

#include "AVDurationSourceMoodSection.generated.h"

class UAVTriggerMoodSectionDataAsset;

Expose_TNameOf(UAVDurationSourceMoodSectionDataAsset);

/** Please add a class description */
UCLASS(Blueprintable, BlueprintType)
class UAVDurationSourceMoodSectionDataAsset : public UAVDurationSourceDataAsset
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAVTriggerMoodSectionDataAsset> SectionTrigger;
};


class AVDurationSourceMoodSection : public IAVDurationSource
{
public:
	static AVDurationSourceMoodSection* Create(UAVDurationSourceDataAsset* InDataAsset);
	
	AVDurationSourceMoodSection(UAVDurationSourceMoodSectionDataAsset* InDataAsset);
	virtual ~AVDurationSourceMoodSection() override;
	
	virtual double GetDuration() override;

private:
	TWeakObjectPtr<UAVDurationSourceMoodSectionDataAsset> DataAsset;
};
