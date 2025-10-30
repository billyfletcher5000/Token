// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/Object.h"
#include "EdGraphUtilities.h"

class UCadenceReactionGroup;
class UCadenceVariable;

class FCadenceReactionGroupCustomization : public IDetailCustomization
{
public:	
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	static void ChangeVariableType(UCadenceVariable* InVariable, const FEdGraphPinType& InNewPinType, TWeakObjectPtr<UCadenceReactionGroup> InGroup, IDetailLayoutBuilder* InLayoutBuilder);
	void OnGenerateVariable(TSharedRef<IPropertyHandle> Property, int32 Index, IDetailChildrenBuilder& ChildrenBuilder, TWeakObjectPtr<UCadenceReactionGroup> InGroup, IDetailLayoutBuilder* InLayoutBuilder);
};

class FCadenceReactionGroupBPPanelPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};