// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "Graph/CadenceGraphNode.h"
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
	void RefreshPins();
	void ReconstructConnections();

	void UpdateRuntimePosition();
	
	void RemoveUserInputPin(UCadenceGraphNodePin* Pin);
	void AddUserInputPin();

	void PostCopy();
	
	UEdGraphPin* GetInputPinByName(const FName& InName);
	UEdGraphPin* GetOutputPinByName(const FName& InName);
	
	TObjectPtr<UCadenceGraphNode> GetRuntimeGraphNode() const { return RuntimeGraphNode; }
	
public:
	// Begin UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitle() : FText::FromString(TEXT("Cadence Base Node")); }
	virtual FLinearColor GetNodeTitleColor() const override { return RuntimeGraphNode ? RuntimeGraphNode->GetNodeTitleColor() : FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual void PrepareForCopying() override;
	virtual void PreEditUndo() override;
	virtual void PostEditUndo() override;
	virtual void ReconstructNode() override;
	// End UEdGraphNode

private:
	void CreatePinInternal(const EEdGraphPinDirection& InDirection, UCadenceGraphNodePin* InPin);

private:
	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> RuntimeGraphNode;
};


class SCadenceGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SCadenceGraphNode){}
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs, UCadenceGraphEditorNode* InNode );
	
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty) override;

protected:
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
};

class SGraphNodeUserAddablePins : public SCadenceGraphNode
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

class FCadenceGraphEditorNodeFactory : public FGraphPanelNodeFactory
{
public:
	// FGraphPanelNodeFactory interface	
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override;
};
