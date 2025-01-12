// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceReactionGroup.generated.h"

class UCadenceVariable;

UCLASS()
class CADENCE_API UCadenceReactionGroup : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	TArray<TObjectPtr<UCadenceVariable>> const& GetVariables() const { return Variables; }

	// Public for detail customization, please use GetVariables() from C++
	UPROPERTY(EditInstanceOnly, Category="Data")
	TArray<TObjectPtr<UCadenceVariable>> Variables;
};
