// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceActorNode.h"
#include "CadenceLatentNode.h"
#include "CadenceMath.h"
#include "Actors/CadenceMeshSplineActor.h"

#include "CadenceGridNodes.generated.h"

namespace FCadenceGridNodeConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Block Grid"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.1f, 0.1f, 0.1f);
	static const FName BlockGridInputPinName = TEXT("Block Grid");
	static const FName PointInputPinName = TEXT("Point");
	static const FName PointAInputPinName = TEXT("Point A");
	static const FName PointBInputPinName = TEXT("Point B");
	static const FName PointProxyAOutputPinName = TEXT("PointProxy A");
	static const FName PointProxyBOutputPinName = TEXT("PointProxy B");
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
	UPROPERTY(EditAnywhere)
	bool bUseNormalisedPositions = true;

	UPROPERTY(EditAnywhere)
	float LineWidth = 1.0f;

	UPROPERTY(EditAnywhere)
	ECadenceSplinePivot PivotType = ECadenceSplinePivot::CentreOfPoints;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="PivotType == ECadenceSplinePivot::SpecificPoint", EditConditionHides))
	int32 PivotSpecificPointIndex = 0;
};

UCLASS()
class CADENCE_API UCadenceGridGetGridNode : public UCadenceActorNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return true; }
	
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

UCLASS()
class CADENCE_API UCadenceGridMoveToPointNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Move To Grid Point")); }
	virtual FText GetNodeCategory() const override { return FCadenceGridNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceGridNodeConstants::NodeTitleColor; }

protected:
	UPROPERTY(EditAnywhere)
	bool bUseNormalisedPositions = true;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECadenceEasingFunc::Type> Easing = ECadenceEasingFunc::Linear;
	
	UPROPERTY(EditAnywhere, DisplayName="(O) Position")
	FVector2D EndPosition;
	
	UPROPERTY(EditAnywhere, DisplayName="(O) Duration")
	float Duration;
};