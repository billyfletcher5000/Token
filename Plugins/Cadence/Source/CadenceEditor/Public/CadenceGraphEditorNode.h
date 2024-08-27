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
	void Construct(TObjectPtr<UCadenceGraphNode> InRuntimeGraphNode);

	
	TObjectPtr<UCadenceGraphNode> GetRuntimeGraphNode() const { return RuntimeGraphNode; }
	
public:
	// Begin UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitle() : FText::FromString(TEXT("Cadence Base Node")); }
	virtual FLinearColor GetNodeTitleColor() const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitleColor() : FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	// End UEdGraphNode
	
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> RuntimeGraphNode;
};
