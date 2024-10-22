// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "CadenceConstantValueNodes.generated.h"


namespace FCadenceConstantValueConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Constants"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.25f, 1.0f, 0.25f);
}

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceConstantValueFloatNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Float Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	float Value;
};

UCLASS()
class CADENCE_API UCadenceConstantValueIntNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Int Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	int32 Value;
};

UCLASS()
class CADENCE_API UCadenceConstantValueBoolNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Bool Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	bool Value;
};

UCLASS()
class CADENCE_API UCadenceConstantValueVectorNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Vector Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	FVector Value;
};


UCLASS()
class CADENCE_API UCadenceConstantValueVector2Node : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Vector2 Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	FVector2D Value;
};

UCLASS()
class CADENCE_API UCadenceConstantValueQuantizationPeriodNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }

	virtual FText GetNodeTitle() const override;
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Quantization Period Constant")); }
	virtual FText GetNodeCategory() const override { return FCadenceConstantValueConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceConstantValueConstants::NodeTitleColor; }
	
private:
	UPROPERTY(EditAnywhere)
	EQuartzCommandQuantization Value;
};