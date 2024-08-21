// Fill out your copyright notice in the Description page of Project Settings.


#include "AVDurationSourceMoodSection.h"

TObjectPtr<AVDurationSourceMoodSection> AVDurationSourceMoodSection::Create(UAVDurationSourceDataAsset* InDataAsset)
{
	return new AVDurationSourceMoodSection(static_cast<UAVDurationSourceMoodSectionDataAsset*>(InDataAsset));
}

AVDurationSourceMoodSection::AVDurationSourceMoodSection(UAVDurationSourceMoodSectionDataAsset* InDataAsset)
: DataAsset(InDataAsset)
{
}

AVDurationSourceMoodSection::~AVDurationSourceMoodSection()
{	
}

double AVDurationSourceMoodSection::GetDuration()
{
	return 0;
}
