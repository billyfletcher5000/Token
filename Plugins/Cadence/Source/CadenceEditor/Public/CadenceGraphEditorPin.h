// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "SGraphPin.h"
#include "Templates/SharedPointer.h"

class UCadenceGraphEditorNode;

class SCadenceGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SCadenceGraphPin) {}		
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPin);
	}

protected:
	virtual FSlateColor GetPinColor() const override { return FSlateColor(FLinearColor::Yellow); }
};

class FCadenceGraphEditorPanelPinFactory :	public FGraphPanelPinFactory
{
public:
	// FGraphPanelPinFactory interface
	virtual TSharedPtr<SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};
