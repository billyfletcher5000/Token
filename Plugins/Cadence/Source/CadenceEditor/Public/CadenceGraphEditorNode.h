// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "CadenceGraphEditorNode.generated.h"

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceGraphEditorNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Cadence Base Node")); }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

public:
	TSharedPtr<UCadenceGraphNode> GetRuntimeGraphNode() const { return RuntimeGraphNode.Pin(); }
	
private:
	TWeakPtr<UCadenceGraphNode> RuntimeGraphNode;
};
