// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceActorLifetime.generated.h"

class UCadenceGraphNode;

UENUM(BlueprintType)
enum class ECadenceActorLifetime : uint8
{
	Automatic	UMETA(ToolTip="Actor is destroyed when all nodes that use the actor have completed"),
	Persist		UMETA(ToolTip="Actor is never automatically destroyed"),
	Graph		UMETA(ToolTip="Actor is destroyed when graph fully completes")
};

USTRUCT()
struct CADENCE_API FCadenceActorLifetimeData
{
	GENERATED_BODY()

	bool operator==(const FCadenceActorLifetimeData& Other) const { return Actor == Other.Actor && Lifetime == Other.Lifetime; }

	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY()
	ECadenceActorLifetime Lifetime = ECadenceActorLifetime::Automatic;

	UPROPERTY()
	TArray<FGuid> UsageGUIDs;
};

UCLASS()
class CADENCE_API UCadenceActorLifetimeManager : public UObject
{
	GENERATED_BODY()

public:
	void RegisterActor(AActor* InActor, const ECadenceActorLifetime& InLifetime, const FGuid& OutputPinVariableGUID);
	void RegisterActorUsage(AActor* InActor, const FGuid& PinVariableGUID, UCadenceGraphNode* InParentNode);
	void UnregisterActorUsage(AActor* InActor, const FGuid& UserGUID);
	void OnGraphEnded();

private:
	TArray<FGuid> GetAutomaticDestroyNodeGUIDs(UCadenceGraphNode* InNode, AActor* InActor);
	void GatherEndPointNodes(TArray<TObjectPtr<UCadenceGraphNode>>& InEndPointNodeList, UCadenceGraphNode* InNode) const;
	
private:
	UPROPERTY()
	TArray<FCadenceActorLifetimeData> Data;
};

inline void UCadenceActorLifetimeManager::OnGraphEnded()
{
	for(FCadenceActorLifetimeData& LifetimeData : Data)
	{
		if(LifetimeData.Lifetime != ECadenceActorLifetime::Persist)
		{
			LifetimeData.Actor->Destroy();
		}
		
		LifetimeData.Actor = nullptr; // This is probably paranoid
	}

	Data.Empty();
}
