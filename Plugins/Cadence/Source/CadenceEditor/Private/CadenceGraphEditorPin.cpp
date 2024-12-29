// Fill out your copyright notice in the Description page of Project Settings.

#include "CadenceGraphEditorPin.h"

#include "EdGraph/EdGraphPin.h"
#include "Graph/CadencePinConstants.h"

#include "KismetPins/SGraphPinBool.h"
#include "KismetPins/SGraphPinEnum.h"
#include "KismetPins/SGraphPinExec.h"
#include "KismetPins/SGraphPinInteger.h"
#include "KismetPins/SGraphPinObject.h"
#include "KismetPins/SGraphPinString.h"
#include "KismetPins/SGraphPinVector.h"
#include "KismetPins/SGraphPinVector2D.h"

TSharedPtr<SGraphPin> FCadenceGraphEditorPanelPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->PinType.IsArray())
		return SNew(SGraphPin, InPin);
	
	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Exec)	
		return SNew(SGraphPinExec, InPin);	

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Integer)	
		return SNew(SGraphPinInteger, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Float)	
		return SNew(SGraphPinNum<float>, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Double)	
		return SNew(SGraphPinNum<double>, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Boolean)	
		return SNew(SGraphPinBool, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_String)	
		return SNew(SGraphPinString, InPin);
	
	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Name)	
		return SNew(SGraphPinString, InPin);
	
	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Text)	
		return SNew(SGraphPinString, InPin);
	
	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Vector)	
		return SNew(SGraphPinVector<double>, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Vector2)	
		return SNew(SGraphPinVector2D<double>, InPin);
	
	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Rotator)	
		return SNew(SGraphPinVector<double>, InPin);

	if (InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Enum && InPin->PinType.PinSubCategoryObject != nullptr && InPin->PinType.PinSubCategoryObject->IsA(UEnum::StaticClass()))
		return SNew(SGraphPinEnum, InPin);

	if(InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Object
		|| InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_Actor
		|| InPin->PinType.PinCategory == FCadencePinCategoryConstants::PC_CadenceAsset)
		return SNew(SGraphPinObject, InPin);

	return SNew(SCadenceGraphPin, InPin);
}
