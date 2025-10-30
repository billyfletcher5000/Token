// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"

#include "AVDurationSource.generated.h"

/** Please add a class description */
UCLASS(Blueprintable, BlueprintType)
class UAVDurationSourceDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
};

class TOKEN_API IAVDurationSource
{
public:	
	virtual ~IAVDurationSource() = default;

	virtual double GetDuration() = 0;
};
