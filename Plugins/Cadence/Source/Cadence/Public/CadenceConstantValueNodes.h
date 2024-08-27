// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "CadenceConstantValueNodes.generated.h"


namespace FCadenceConstantValueConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Constants"));
}

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceConstantValueFloatNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual bool Execute() override;

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Float Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	
private:
	UPROPERTY(EditAnywhere)
	float Value;
};
