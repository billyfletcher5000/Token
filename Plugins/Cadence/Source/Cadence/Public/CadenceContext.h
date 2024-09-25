// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceContext.generated.h"

class UCadenceAssetInstance;
class UCadenceGraphNode;
class UCadenceActorLifetimeManager;
class UCadenceAsset;
class UCadenceGraphRunnerPathway;
class UCadenceGraphRunner;
class UCadenceGraph;
class UCadenceActorSettings;

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
	TObjectPtr<UCadenceGraph> Graph;

	UPROPERTY()
	TObjectPtr<UCadenceAsset> Asset;

	UPROPERTY()
	TObjectPtr<UCadenceAssetInstance> AssetInstance;

	UPROPERTY()
	TObjectPtr<UCadenceGraph> SourceGraph;

	UPROPERTY()
	TObjectPtr<UCadenceGraphRunner> Runner;

	UPROPERTY()
	TObjectPtr<UCadenceGraphRunnerPathway> Pathway;

	UPROPERTY()
	TObjectPtr<UCadenceActorLifetimeManager> ActorLifetimeManager;

	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> ParentNode = nullptr;

	UPROPERTY()
	float DeltaSeconds = 0.0f;

	UPROPERTY()
	bool bProcessNodesImmediately = true;
};
