// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceDelayNodes.generated.h"

class UCadenceGraphRunnerPathway;

namespace FCadenceDelayConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Delays"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.3f, 0.6f, 0.9f);
}

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceDelayNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual bool Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Delay")); }
	virtual FText GetNodeCategory() const override { return FCadenceDelayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDelayConstants::NodeTitleColor; }

private:
	TMap<UCadenceGraphRunnerPathway*, float> TimeElapsedMap;
};
