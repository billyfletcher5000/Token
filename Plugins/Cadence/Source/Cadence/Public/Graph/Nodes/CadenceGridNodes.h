// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceActorNode.h"

#include "CadenceGridNodes.generated.h"

namespace FCadenceGridNodeConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Block Grid"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.1f, 0.1f, 0.1f);
}

UCLASS()
class CADENCE_API UCadenceGridCreateLineNode : public UCadenceActorNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Create Grid Line")); }
	virtual FText GetNodeCategory() const override { return FCadenceGridNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceGridNodeConstants::NodeTitleColor; }

protected:	
	static const FName BlockGridInputPinName;
	static const FName PointAInputPinName;
	static const FName PointBInputPinName;
};

UCLASS()
class CADENCE_API UCadenceGridGetGridNode : public UCadenceActorNode
{
	GENERATED_BODY()

public:
	//virtual bool IsPure() const override { return true; }
	
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Get Grid")); }
	virtual FText GetNodeCategory() const override { return FCadenceGridNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceGridNodeConstants::NodeTitleColor; }

protected:	
	static const FName BlockGridInputPinName;
	static const FName PointAInputPinName;
	static const FName PointBInputPinName;
};