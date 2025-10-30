// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceGraphGridPreviewCommandDecorators.generated.h"

class UCadenceGridPreviewDrawCommand;

struct FCadenceGridPreviewDrawSettings
{
	FMargin Padding;
};

UCLASS(Abstract)
class UCadenceGraphGridPreviewCommandDecorator : public UObject
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UCadenceGridPreviewDrawCommand> GetTargetCommandType() const PURE_VIRTUAL(UCadenceGraphGridPreviewCommandDecorator::GetTargetCommandType, return nullptr;);
	virtual TSharedRef<SWidget> CreateVisualDisplay(UCadenceGridPreviewDrawCommand* InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings) PURE_VIRTUAL(UCadenceGraphGridPreviewCommandDecorator::CreateVisualDisplay, return SNullWidget::NullWidget;);
};

UCLASS()
class UCadenceGraphGridPreviewPointCommandDecorator : public UCadenceGraphGridPreviewCommandDecorator
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UCadenceGridPreviewDrawCommand> GetTargetCommandType() const;
	virtual TSharedRef<SWidget> CreateVisualDisplay(UCadenceGridPreviewDrawCommand* InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings);
};


UCLASS()
class UCadenceGraphGridPreviewLineCommandDecorator : public UCadenceGraphGridPreviewCommandDecorator
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UCadenceGridPreviewDrawCommand> GetTargetCommandType() const;
	virtual TSharedRef<SWidget> CreateVisualDisplay(UCadenceGridPreviewDrawCommand* InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings);
};