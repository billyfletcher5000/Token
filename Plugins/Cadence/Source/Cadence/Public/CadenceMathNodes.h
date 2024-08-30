// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphAddPinInterface.h"
#include "CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceMathNodes.generated.h"

namespace FCadenceMathNodeConstants
{	
	static const FText NodeCategory = FText::FromString(TEXT("Maths"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.1f, 1.0f, 0.1f);
}

UCLASS()
class CADENCE_API UCadenceAddNode : public UCadenceGraphNode, ICadenceGraphAddPinInterface
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	
	virtual bool Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }
	
	virtual TObjectPtr<UCadenceGraphNodePin> AddUserInputPin() override;
	virtual bool RemoveUserInputPin(UCadenceGraphNodePin* Pin) override;
	virtual bool CanRemovePin(const UCadenceGraphNodePin* Pin) const override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Add")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }

private:
	UPROPERTY()
	int32 PinIndex = 0;
};
