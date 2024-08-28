// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceAsset.generated.h"

class UCadenceGraph;
/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceAsset : public UObject
{
	GENERATED_BODY()

public:
	void CreateGraph();
	UCadenceGraph* GetGraph() const { return Graph; } 
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraph> Graph;	
};
