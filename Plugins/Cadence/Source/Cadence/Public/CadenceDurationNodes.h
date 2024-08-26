// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "CadenceDurationNodes.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceQuantizedDurationNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual bool Execute() override;

public:
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization TimePeriod;
};
