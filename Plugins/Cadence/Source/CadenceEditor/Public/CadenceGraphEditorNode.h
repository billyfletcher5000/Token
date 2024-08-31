// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
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
	void ReconstructConnections();

	void UpdateRuntimePosition();
	
	void RemoveUserInputPin(UCadenceGraphNodePin* Pin);
	void AddUserInputPin();

	UEdGraphPin* GetInputPinByName(const FName& InName);
	UEdGraphPin* GetOutputPinByName(const FName& InName);
	
	TObjectPtr<UCadenceGraphNode> GetRuntimeGraphNode() const { return RuntimeGraphNode; }
	
public:
	// Begin UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitle() : FText::FromString(TEXT("Cadence Base Node")); }
	virtual FLinearColor GetNodeTitleColor() const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitleColor() : FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	// End UEdGraphNode

private:
	void CreatePinInternal(const EEdGraphPinDirection& InDirection, UCadenceGraphNodePin* InPin);
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> RuntimeGraphNode;
};

class SGraphNodeUserAddablePins : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeUserAddablePins){}
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs, UCadenceGraphEditorNode* InNode );

protected:
	// SGraphNode interface
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual FReply OnAddPin() override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	// End of SGraphNode interface
};

class FCadenceGraphEditorNodeFactory : public FGraphNodeFactory
{
public:
	// FGrapNodeFactory interface	
	virtual TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override;
};
