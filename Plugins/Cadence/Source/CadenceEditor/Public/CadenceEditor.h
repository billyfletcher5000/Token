// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UCadenceVariable;
class FCadenceGraphEditorNodeFactory;
class FCadenceGraphEditorPanelPinFactory;

DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<SWidget>, FVariableCreateInlineWidgetFunc, UCadenceVariable* /* InVariable */,
																					 TSharedPtr<IPropertyHandle> /* InPropertyHandle */);

typedef TMap<FName /* Class Name */, FVariableCreateInlineWidgetFunc> VariableInlineWidgetFuncMap;

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

	VariableInlineWidgetFuncMap& GetMap() { return VariableToInlineWidgetFunc; }
	VariableInlineWidgetFuncMap VariableToInlineWidgetFunc;
	
private:
	TSharedPtr<FSlateStyleSet> StyleSet = nullptr;
	TSharedPtr<FCadenceGraphEditorPanelPinFactory> PanelPinFactory = nullptr;
	TSharedPtr<FCadenceGraphEditorNodeFactory> NodeFactory = nullptr;	
};
