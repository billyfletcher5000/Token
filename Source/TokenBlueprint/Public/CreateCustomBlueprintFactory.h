// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "CreateCustomBlueprintFactory.generated.h"

/**
 * 
 */
UCLASS()
class TOKENBLUEPRINT_API UCreateCustomBlueprintFactory : public UK2Node
{
	GENERATED_BODY()

public:
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
};
