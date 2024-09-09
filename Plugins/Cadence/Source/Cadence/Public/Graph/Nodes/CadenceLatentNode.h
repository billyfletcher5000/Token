// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphRunner.h"
#include "TickableActions/ICadenceTickableAction.h"
#include "CadenceLatentNode.generated.h"

class UCadenceGraphRunnerPathway;

USTRUCT()
struct FCadencePathwayActionListPair
{
	GENERATED_BODY()
	virtual ~FCadencePathwayActionListPair() = default;

	static FCadencePathwayActionListPair Create(UCadenceGraphRunnerPathway* InPathway, const TArray<TScriptInterface<ICadenceTickableAction>>& InActionList)
	{
		FCadencePathwayActionListPair Pair;
		Pair.Pathway = InPathway;
		Pair.ActionList = InActionList;
		return Pair;
	}
	
	bool operator==(const FCadencePathwayActionListPair& Other) const
	{
		return Pathway == Other.Pathway;
	}
	bool operator==(const UCadenceGraphRunnerPathway* Other) const
	{
		return Pathway == Other;
	}
	
	UPROPERTY()
	UCadenceGraphRunnerPathway* Pathway;

	UPROPERTY()
	TArray<TScriptInterface<ICadenceTickableAction>> ActionList;
};

UCLASS(Abstract)
class CADENCE_API UCadenceLatentNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsLatent() const override final { return true; }
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
protected:
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList) PURE_VIRTUAL();

private:
	UPROPERTY()
	TArray<FCadencePathwayActionListPair> ActionListMap;
};
