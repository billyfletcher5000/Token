// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceSynesthesiaNodes.generated.h"

namespace FCadenceSynesthesiaConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Audio Synesthesia"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.5f, 0.2f, 0.35f);

	static const FName Pin_LoudnessNRT = TEXT("Loudness NRT");
	static const FName Pin_Loudness = TEXT("Loudness");
	
	static const FName Pin_ConstantQNRT = TEXT("ConstantQ NRT");
	static const FName Pin_ConstantQ = TEXT("ConstantQ");
	
	static const FName Pin_OnsetNRT = TEXT("Onset NRT");
	static const FName Pin_OnsetTimestamps = TEXT("Onset Timestamps");
	static const FName Pin_OnsetStrengths = TEXT("Onset Strengths");
	
	static const FName Pin_Seconds = TEXT("Seconds");
	static const FName Pin_StartSeconds = TEXT("Start Seconds");
	static const FName Pin_EndSeconds = TEXT("End Seconds");
	static const FName Pin_Channel = TEXT("Channel");
	static const FName Pin_Normalized = TEXT("Normalized");
}

UCLASS()
class CADENCE_API UCadenceGetLoudnessAtTimeNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Get Loudness At Time")); }
	virtual FText GetNodeCategory() const override { return FCadenceSynesthesiaConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceSynesthesiaConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceGetConstantQAtTimeNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Get ConstantQ At Time")); }
	virtual FText GetNodeCategory() const override { return FCadenceSynesthesiaConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceSynesthesiaConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceGetOnsetsBetweenTimesNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Get Onsets Between Times")); }
	virtual FText GetNodeCategory() const override { return FCadenceSynesthesiaConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceSynesthesiaConstants::NodeTitleColor; }
};
