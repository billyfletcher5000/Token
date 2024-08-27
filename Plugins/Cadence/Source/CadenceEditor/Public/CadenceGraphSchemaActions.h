// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "CadenceGraphSchemaActions.generated.h"

class UCadenceGraphNode;

USTRUCT()
struct FNewNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewNodeAction() {}
	FNewNodeAction(TSubclassOf<UCadenceGraphNode> InRuntimeNodeType, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), RuntimeNodeType(InRuntimeNodeType)
	{		
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	
private:
	UPROPERTY()
	TSubclassOf<UCadenceGraphNode> RuntimeNodeType;
};