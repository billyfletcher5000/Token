// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"

#include "CadenceReactionNodes.generated.h"

namespace FCadenceReactionNodeConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Reaction"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.25f, 0.0f);
}

UCLASS()
class CADENCE_API UCadenceSetReactionGroupValuesNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Set Reaction Group Values")); }
	virtual FText GetNodeCategory() const override { return FCadenceReactionNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceReactionNodeConstants::NodeTitleColor; }

private:
	UFUNCTION()
	void UpdateVariableInputPins();
	
	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> VariableInputPins;
	
	TWeakObjectPtr<UCadenceVariable> ValueChangedVariable;
};

UCLASS()
class CADENCE_API UCadenceBreakReactionGroupNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Break Reaction Group")); }
	virtual FText GetNodeCategory() const override { return FCadenceReactionNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceReactionNodeConstants::NodeTitleColor; }

private:
	UFUNCTION()
	void UpdateVariableOutputPins();
	
	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> VariableInputPins;

	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> VariableOutputPins;
	
	TWeakObjectPtr<UCadenceVariable> ValueChangedVariable;
};