// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceGraphGridPreviewCommandDecorators.generated.h"

struct FCadenceGridPreviewDrawCommand;

struct FCadenceGridPreviewDrawSettings
{
	FMargin Padding;
};

UCLASS(Abstract)
class UCadenceGraphGridPreviewCommandDecorator : public UObject
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetTargetCommandType() const PURE_VIRTUAL(UCadenceGraphGridPreviewCommandDecorator::GetTargetCommandType, return nullptr;);
	virtual TSharedRef<SWidget> CreateVisualDisplay(const FCadenceGridPreviewDrawCommand& InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings) PURE_VIRTUAL(UCadenceGraphGridPreviewCommandDecorator::CreateVisualDisplay, return SNullWidget::NullWidget;);
};