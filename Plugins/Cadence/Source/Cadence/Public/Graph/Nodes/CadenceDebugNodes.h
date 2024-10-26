// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "CadenceDebugNodes.generated.h"

namespace FCadenceDebugConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Debug"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.5f, 0.5f);
}

UCLASS()
class CADENCE_API UCadenceDebugIntNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Debug Int")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceDebugFloatNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Debug Float")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};


UCLASS()
class CADENCE_API UCadenceDebugStringNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Debug String")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceDebugActorNameNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Debug Actor Name")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceDebugQuantizationPeriodNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Debug Enum")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};


UCLASS()
class CADENCE_API UCadenceTestIncrementIntNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Test Increment Int")); }
	virtual FText GetNodeCategory() const override { return FCadenceDebugConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceDebugConstants::NodeTitleColor; }
};