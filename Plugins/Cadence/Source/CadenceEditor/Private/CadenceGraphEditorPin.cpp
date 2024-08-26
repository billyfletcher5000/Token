// Fill out your copyright notice in the Description page of Project Settings.

#include "CadenceGraphEditorPin.h"

#include "CadenceGraphSchema.h"
#include "EdGraph/EdGraphPin.h"

TSharedPtr<SGraphPin> FCadenceGraphEditorPanelPinFactory::CreatePin(UEdGraphPin* InPin) const
{	
	if (const UCadenceGraphSchema* CadenceGraphSchema = Cast<const UCadenceGraphSchema>(InPin->GetSchema()))
	{
		return SNew(SCadenceGraphPin, InPin);
	}

	return nullptr;
}
