// Fill out your copyright notice in the Description page of Project Settings.

#include "CadenceGraphEditorPin.h"

#include "CadenceGraphSchema.h"
#include "EdGraph/EdGraphPin.h"
#include "KismetPins/SGraphPinExec.h"

TSharedPtr<SGraphPin> FCadenceGraphEditorPanelPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UCadenceGraphSchema::PC_Exec)
	{
		return SNew(SGraphPinExec, InPin);
	}
	
	if (InPin->PinType.PinCategory == UCadenceGraphSchema::PC_Variable)
	{
		return SNew(SCadenceGraphPin, InPin);
	}

	return nullptr;
}
