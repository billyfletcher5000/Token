// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "CadenceDurationNodes.generated.h"

namespace FCadenceDurationConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Duration"));
}

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

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Quantized Duration")); }
	virtual FText GetNodeCategory() const override { return FCadenceDurationConstants::NodeCategory; }

public:
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization TimePeriod;
};
