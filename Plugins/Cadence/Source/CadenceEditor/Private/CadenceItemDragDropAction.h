// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphEditorDragDropAction.h"
#include "UObject/Object.h"

class FCadenceGraphApplication;
class UCadenceGraph;
class UCadenceVariable;
class UCadenceAsset;

class FCadenceItemDragDropAction : public FGraphSchemaActionDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FCadenceItemDragDropAction, FGraphSchemaActionDragDropAction)

	// FGraphEditorDragDropAction interface
	virtual FReply DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action) override;
	virtual FReply DroppedOnCategory(FText Category) override;
	virtual FReply DroppedOnPanel(const TSharedRef< SWidget >& Panel, const FVector2f& ScreenPosition, const FVector2f& GraphPosition, UEdGraph& Graph) override;
	virtual void HoverTargetChanged() override;
	// End of FGraphEditorDragDropAction

	/** Set if operation is modified by alt */
	void SetAltDrag(bool InIsAltDrag) {	bAltDrag = InIsAltDrag; }

	/** Set if operation is modified by the ctrl key */
	void SetCtrlDrag(bool InIsCtrlDrag) { bControlDrag = InIsCtrlDrag; }

	
	DECLARE_DELEGATE_RetVal_ThreeParams(bool, FCanBeDroppedDelegate, TSharedPtr<FEdGraphSchemaAction> /*DropAction*/, UEdGraph* /*HoveredGraphIn*/, FText& /*ImpededReasonOut*/);
	static TSharedRef<FCadenceItemDragDropAction> New(TSharedPtr<FEdGraphSchemaAction> InActionNode, FCanBeDroppedDelegate CanBeDroppedDelegate)
	{
		TSharedRef<FCadenceItemDragDropAction> Operation = MakeShareable(new FCadenceItemDragDropAction);
		Operation->SourceAction = InActionNode;
		Operation->CanBeDroppedDelegate = CanBeDroppedDelegate;
		Operation->Construct();
		return Operation;
	}

protected:
	/** Constructor */
	FCadenceItemDragDropAction();

	virtual UCadenceAsset* GetSourceAsset() const
	{
		return nullptr;
	}

	/** Helper method to see if we're dragging in the same blueprint */
	bool IsFromAsset(UCadenceAsset* InAsset) const
	{
		return GetSourceAsset() == InAsset;
	}

	void SetFeedbackMessageError(const FText& Message);
	void SetFeedbackMessageOK(const FText& Message);
	
	bool ActionWillShowExistingNode() const;

protected:
	/** Was ctrl held down at start of drag */
	bool bControlDrag;
	/** Was alt held down at the start of drag */
	bool bAltDrag;

	FCanBeDroppedDelegate CanBeDroppedDelegate;
};

class FCadenceVariableItemDragDropAction : public FCadenceItemDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FCadenceVariableItemDragDropAction, FCadenceItemDragDropAction)
		
	// FGraphEditorDragDropAction interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnPanel(const TSharedRef< class SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph) override;
	// End of FGraphEditorDragDropAction

	static TSharedRef<FCadenceVariableItemDragDropAction> New(TSharedPtr<FEdGraphSchemaAction> InAction, UCadenceVariable* InVariable, UCadenceAsset* InAsset, TSharedPtr<FCadenceGraphApplication> InApplication);

	UCadenceVariable* GetVariable()
	{
		return Variable.Get();
	}

protected:
	 /** Construct a FKismetVariableDragDropAction */
	FCadenceVariableItemDragDropAction();

	/** Structure for required node construction parameters */
	struct FNodeConstructionParams
	{
		FVector2D GraphPosition;
		UCadenceGraph* Graph;
		UCadenceVariable* Variable;
		UEdGraph* EdGraph;
	};

	// FGraphSchemaActionDragDropAction interface
	virtual void GetDefaultStatusSymbol(const FSlateBrush*& PrimaryBrushOut, FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut) const override;
	// End of FGraphSchemaActionDragDropAction interface

	// FMyBlueprintItemDragDropAction interface
	virtual UCadenceAsset* GetSourceAsset() const override;
	// End of FMyBlueprintItemDragDropAction interface

	/** Called when user selects to create a Getter for the variable */
	static void MakeGetter(FNodeConstructionParams InParams);
	/** Called when user selects to create a Setter for the variable */
	static void MakeSetter(FNodeConstructionParams InParams);
	/** Called too check if we can execute a setter on a given property */
	static bool CanExecuteMakeSetter(FNodeConstructionParams InParams, UCadenceVariable* InVariable);

	/**
	 * Checks if the property can be dropped in a graph
	 *
	 * @param InVariableProperty		The variable property to check with
	 * @param InGraph					The graph to check against placing the variable
	 */
	bool CanVariableBeDropped(const UCadenceVariable* InVariable, const UEdGraph& InGraph) const;

protected:
	/** Scope this variable belongs to */
	TWeakObjectPtr<UCadenceVariable> Variable;

	TWeakObjectPtr<UCadenceAsset> Asset;

	TWeakPtr<FCadenceGraphApplication> Application;
};