// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCadenceGraphEditorNodeFactory;
class FCadenceGraphEditorPanelPinFactory;

class FCadenceEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static const FName CategoryKey;
	static const FText CategoryDisplayName;
	static const FName StyleSetName;

private:
	TSharedPtr<FSlateStyleSet> StyleSet = nullptr;
	TSharedPtr<FCadenceGraphEditorPanelPinFactory> PanelPinFactory = nullptr;
	TSharedPtr<FCadenceGraphEditorNodeFactory> NodeFactory = nullptr;
};
