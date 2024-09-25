// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "CadenceDurationNodes.generated.h"

struct FCadenceSectionName;

namespace FCadenceDurationConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Duration"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.25f, 0.0f);
}

UCLASS()
class CADENCE_API UCadenceQuantizedDurationNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Quantized Duration")); }
	virtual FText GetNodeCategory() const override { return FCadenceDurationConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDurationConstants::NodeTitleColor; }

public:
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization TimePeriod;
};

UCLASS()
class CADENCE_API UCadenceSequenceSectionDurationNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Sequence Section Duration")); }
	virtual FText GetNodeCategory() const override { return FCadenceDurationConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDurationConstants::NodeTitleColor; }

public:
	UPROPERTY(EditAnywhere)
	FCadenceSectionName Section;
};
