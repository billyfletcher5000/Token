// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "CadenceDebugNodes.generated.h"

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceDebugFloatNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual bool Execute() override;	
};
