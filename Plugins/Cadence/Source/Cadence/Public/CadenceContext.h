// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceContext.generated.h"

class UCadenceAsset;
class UCadenceGraphRunnerPathway;
class UCadenceGraphRunner;
class UCadenceGraph;

/**
 * Context object passed to all executing nodes, primarily for them to get track specific information
 * or other shared graph instance data
 *
 * Could conceivably be stackable (e.g. CreateChildContext() style) per execution path if needed
 */
UCLASS()
class CADENCE_API UCadenceContext : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName TrackName;

	UPROPERTY()
	UCadenceGraph* Graph;

	UPROPERTY()
	UCadenceAsset* Asset;

	UPROPERTY()
	UCadenceGraph* SourceGraph;

	UPROPERTY()
	UCadenceGraphRunner* Runner;

	UPROPERTY()
	UCadenceGraphRunnerPathway* Pathway;

	UPROPERTY()
	float DeltaSeconds = 0.0f;
};
