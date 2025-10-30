// Copyright (C) Billy Fletcher 2025


#include "MoodSection/AVDurationSourceMoodSection.h"


AVDurationSourceMoodSection* AVDurationSourceMoodSection::Create(UAVDurationSourceDataAsset* InDataAsset)
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
