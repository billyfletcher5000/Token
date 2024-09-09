// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"

#include "CadenceUtilityNodes.generated.h"

namespace FCadenceUtilityNodeConstants
{	
	static const FText NodeCategory = FText::FromString(TEXT("Utility"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.1f, 0.75f, 1.0f);
}

UCLASS()
class CADENCE_API UCadenceKeepAliveActorNode : public UCadenceGraphNode
{
	GENERATED_BODY()
	
public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Actor Alive Utility")); }
	virtual FText GetNodeCategory() const override { return FCadenceUtilityNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceUtilityNodeConstants::NodeTitleColor; }
};
