// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraph.h"
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

protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location);
	
private:
	UPROPERTY()
	TSubclassOf<UCadenceGraphNode> RuntimeNodeType;
};

USTRUCT()
struct FNewVariableGetterNodeAction : public FNewNodeAction
{
	GENERATED_BODY()

public:
	FNewVariableGetterNodeAction() {}
	FNewVariableGetterNodeAction(FCadenceNamedVariable& InNamedVariable, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FNewNodeAction(nullptr, InNodeCategory, InMenuDesc, InToolTip, InGrouping), NamedVariable(InNamedVariable)
	{		
	}
	
protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location) override;
	
private:
	UPROPERTY()
	FCadenceNamedVariable NamedVariable;
};

USTRUCT()
struct FNewVariableSetterNodeAction : public FNewNodeAction
{
	GENERATED_BODY()

public:
	FNewVariableSetterNodeAction() {}
	FNewVariableSetterNodeAction(FCadenceNamedVariable& InNamedVariable, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FNewNodeAction(nullptr, InNodeCategory, InMenuDesc, InToolTip, InGrouping), NamedVariable(InNamedVariable)
	{		
	}
	
protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location) override;
	
private:
	UPROPERTY()
	FCadenceNamedVariable NamedVariable;
};