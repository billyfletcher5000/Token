// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphEditorNode.h"

#include "CadenceGraphEditorGridNode.generated.h"

class UCadenceGridPreviewDrawCommand;

UCLASS()
class CADENCEEDITOR_API UCadenceGraphEditorGridNode : public UCadenceGraphEditorNode
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;

	void GetPreviewDrawCommands(TArray<UCadenceGridPreviewDrawCommand*>& InDrawCommandList);

	virtual bool CanDisplayGridPreview() const { return true; } 
	bool IsGridPreviewExpanded() const { return bIsGridPreviewExpanded; }
	void SetGridPreviewExpanded(const bool& bIsExpanded) { bIsGridPreviewExpanded = bIsExpanded; }
	
	virtual FText GetPreviewOverlayText() const { return FText(); }

protected:
	UPROPERTY()
	bool bIsGridPreviewExpanded = false;
};

class SCadenceGraphGridNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SCadenceGraphGridNode){}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UCadenceGraphEditorGridNode* InNode);

protected:
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override;
	virtual void SetDefaultTitleAreaWidget(TSharedRef<SOverlay> DefaultTitleAreaWidget) override;
	virtual void OnAdvancedViewChanged(ECheckBoxState NewCheckedState) override;

	TSharedRef<SWidget> CreatePreviewWidget();
	TSharedRef<SWidget> CreateGridBaseWidget();
	void OnPreviewExpandedChanged(ECheckBoxState InCheckBoxState);	
	const FSlateBrush* GetGridPreviewArrow();
	FText GetPreviewOverlayText() const;	

private:
	UCadenceGraphEditorGridNode* GridNode = nullptr;
};
