// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Constants used for metadata, etc... 
struct CADENCEEDITOR_API FCadenceMetadata
{
public:
	/** Metadata to identify a Reaction Group Pin. Depending on which Reaction Group is selected, we display different RowName options */
	static const FName MD_ReactionGroupPin;
};

const FName FCadenceMetadata::MD_ReactionGroupPin = FName(TEXT("ReactionGroupPin"));