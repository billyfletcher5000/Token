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
	virtual bool Execute() override;

	virtual void AddUserInputPin() override;
	virtual void RemoveUserInputPin(UCadenceGraphNodePin* Pin) override;
	virtual bool CanRemovePin(const UCadenceGraphNodePin* Pin) const;
	
	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Trigger Sequence")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }

private:
	UPROPERTY()
	int32 PinIndex = 0;
};
