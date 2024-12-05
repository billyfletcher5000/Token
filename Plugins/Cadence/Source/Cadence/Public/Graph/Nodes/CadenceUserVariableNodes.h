// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceVariable.h"
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
	static FText GetGetterNodeMenuName(const UCadenceVariable* NamedVariable)
	{		
		return FText::FromString(FCadenceUserVariableConstants::GetterPrefix + NamedVariable->GetUserVariableName().ToString());
	}
	
	static FText GetSetterNodeMenuName(const UCadenceVariable* NamedVariable)
	{		
		return FText::FromString(FCadenceUserVariableConstants::SetterPrefix + NamedVariable->GetUserVariableName().ToString());
	}
}

UCLASS(Abstract)
class CADENCE_API UCadenceUserVariableAccessNode : public UCadenceGraphNode
{
	GENERATED_BODY()
	
public:	
	virtual void SetSourceVariable(UCadenceVariable* InVariable) { SourceVariable = InVariable; }
	virtual UCadenceVariable* GetSourceVariable() { return SourceVariable; }
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const override { return false; }
#endif
	
protected:
	UPROPERTY()
	TObjectPtr<UCadenceVariable> SourceVariable;
};

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceUserVariableGetterNode : public UCadenceUserVariableAccessNode
{
	GENERATED_BODY()
	
public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	UCadenceGraphNodePin* GetVariableOutputPin() const;

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override;
	virtual FText GetNodeCategory() const override { return FCadenceUserVariableConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceUserVariableConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceUserVariableSetterNode : public UCadenceUserVariableAccessNode
{
	GENERATED_BODY()
	
public:	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	UCadenceGraphNodePin* GetVariableInputPin() const;
	UCadenceGraphNodePin* GetVariableOutputPin() const;

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override;
	virtual FText GetNodeCategory() const override { return FCadenceUserVariableConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceUserVariableConstants::NodeTitleColor; }
};