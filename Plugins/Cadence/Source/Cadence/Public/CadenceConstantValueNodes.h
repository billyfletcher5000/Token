// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "CadenceConstantValueNodes.generated.h"

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

private:
	UPROPERTY(EditAnywhere)
	float Value;
};
