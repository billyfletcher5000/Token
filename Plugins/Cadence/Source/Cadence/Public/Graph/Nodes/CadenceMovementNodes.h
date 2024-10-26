// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceLatentNode.h"
#include "CadenceMath.h"
#include "UObject/Object.h"
#include "CadenceMovementNodes.generated.h"

namespace FCadenceMovementConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Movement"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.4f, 0.1f);
}

UCLASS()
class CADENCE_API UCadenceRotateToNode : public UCadenceLatentNode
{
	GENERATED_BODY()

public:	
	virtual void CreateInputPins() override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Rotate To")); }
	virtual FText GetNodeCategory() const override { return FCadenceMovementConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMovementConstants::NodeTitleColor; }

protected:
	virtual void CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext) override;

	virtual FRotator TransformRotation(const FRotator& InRotation, AActor* InActor) { return InRotation; }

protected:
	UPROPERTY(EditAnywhere)
	bool bUseShortestPath = true;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECadenceEasingFunc::Type> Easing = ECadenceEasingFunc::Linear;
};

UCLASS()
class CADENCE_API UCadenceRotateByNode : public UCadenceRotateToNode
{
	GENERATED_BODY()

public:	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Rotate By")); }

protected:	
	virtual FRotator TransformRotation(const FRotator& InRotation, AActor* InActor) override { return InActor->GetActorRotation() + InRotation; }
};