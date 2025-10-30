// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceLatentNode.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceDelayNodes.generated.h"

class UCadenceGraphRunnerPathway;

namespace FCadenceDelayConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Delays"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.3f, 0.6f, 0.9f);
}

UCLASS()
class CADENCE_API UCadenceDelayNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Delay")); }
	virtual FText GetNodeCategory() const override { return FCadenceDelayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDelayConstants::NodeTitleColor; }

protected:
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;
};
