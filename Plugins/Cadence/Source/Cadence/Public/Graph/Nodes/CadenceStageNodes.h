// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceLatentNode.h"
#include "UObject/Object.h"
#include "CadenceStageNodes.generated.h"

namespace FCadenceStageNodeConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Stage"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.3f, 0.6f, 0.9f);

	static const FName Pin_Snapshot = TEXT("Snapshot");
	static const FName Pin_Transition = TEXT("Transition");
}

UCLASS()
class CADENCE_API UCadenceStageTransitionNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Stage Transition")); }
	virtual FText GetNodeCategory() const override { return FCadenceStageNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceStageNodeConstants::NodeTitleColor; }

protected:
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;
};