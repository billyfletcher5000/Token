// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphAddPinInterface.h"
#include "CadenceGraphNode.h"
#include "CadenceTriggerNodes.generated.h"

namespace FCadenceTriggerConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Triggers"));
	static const FLinearColor NodeTitleColor = FLinearColor(0.75f, 0.0f, 0.0f);
}

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceTriggerSequenceNode : public UCadenceGraphNode, public ICadenceGraphAddPinInterface
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual bool Execute(UCadenceContext* InContext) override;

	virtual TObjectPtr<UCadenceGraphNodePin> AddUserInputPin() override;
	virtual bool RemoveUserInputPin(UCadenceGraphNodePin* Pin) override;
	virtual bool CanRemovePin(const UCadenceGraphNodePin* Pin) const override;
	
	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Trigger Sequence")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }

private:
	UPROPERTY()
	int32 PinIndex = 0;
};

UCLASS()
class CADENCE_API UCadenceQuantizedTimeTriggerNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual bool Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Quantized Time Trigger")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }

public:
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization TimePeriod;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};