// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceFlowNodes.generated.h"

namespace FCadenceFlowControlConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Flow Control"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.5f, 0.5f);
}

UCLASS()
class CADENCE_API UCadenceBranchNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Branch")); }
	virtual FText GetNodeCategory() const override { return FCadenceFlowControlConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceFlowControlConstants::NodeTitleColor; }

	virtual bool ShouldCreateThenExecPin() const override { return false; }
	virtual TArray<UCadenceGraphNodePin*> GetActuatingOutputExecPins() const override;
};