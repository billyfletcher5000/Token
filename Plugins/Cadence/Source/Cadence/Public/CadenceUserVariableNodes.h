// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraph.h"
#include "CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceUserVariableNodes.generated.h"

namespace FCadenceUserVariableConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("User Variables"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.25f, 0.25f, 1.0f);
	static const FString GetterPrefix = TEXT("Get ");
	static const FString SetterPrefix = TEXT("Set ");
}

namespace FCadenceUserVariableHelper
{
	static FText GetGetterNodeMenuName(const FCadenceNamedVariable& NamedVariable)
	{		
		return FText::FromString(FCadenceUserVariableConstants::GetterPrefix + NamedVariable.Name.ToString());
	}
	
	static FText GetSetterNodeMenuName(const FCadenceNamedVariable& NamedVariable)
	{		
		return FText::FromString(FCadenceUserVariableConstants::SetterPrefix + NamedVariable.Name.ToString());
	}
}

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceUserVariableGetterNode : public UCadenceGraphNode
{
	GENERATED_BODY()
	
public:
	virtual void CreateOutputPins() override;
	virtual bool Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override;
	virtual FText GetNodeCategory() const override { return FCadenceUserVariableConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceUserVariableConstants::NodeTitleColor; }

	void SetSourceVariable(FCadenceNamedVariable& InVariable) { SourceVariable = InVariable; }
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const override { return false; }
#endif
	
private:
	UPROPERTY()
	FCadenceNamedVariable SourceVariable;
};

UCLASS()
class CADENCE_API UCadenceUserVariableSetterNode : public UCadenceGraphNode
{
	GENERATED_BODY()
	
public:	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual bool Execute(UCadenceContext* InContext) override;

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override;
	virtual FText GetNodeCategory() const override { return FCadenceUserVariableConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceUserVariableConstants::NodeTitleColor; }

	void SetSourceVariable(FCadenceNamedVariable& InVariable) { SourceVariable = InVariable; }

#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const override { return false; }
#endif

private:
	UPROPERTY()
	FCadenceNamedVariable SourceVariable;
};