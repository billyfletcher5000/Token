// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceLatentNode.h"
#include "Graph/CadenceGraphAddPinInterface.h"
#include "Graph/CadenceGraphNode.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "CadenceTriggerNodes.generated.h"

namespace FCadenceTriggerConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Triggers"));
	static const FLinearColor NodeTitleColor = FLinearColor(0.75f, 0.0f, 0.0f);
}

UCLASS()
class CADENCE_API UCadenceWaitForTriggerNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Wait For Trigger")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }

protected:
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;
};

UCLASS()
class CADENCE_API UCadenceTriggerSequenceNode : public UCadenceGraphNode, public ICadenceGraphAddPinInterface
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

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
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Quantized Time Trigger")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceSequenceSectionTriggerNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Sequence Section Trigger")); }
	virtual FText GetNodeCategory() const override { return FCadenceTriggerConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceTriggerConstants::NodeTitleColor; }
};