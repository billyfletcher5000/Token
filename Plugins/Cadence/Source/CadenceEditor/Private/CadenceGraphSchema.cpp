// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchema.h"

#include "CadenceGraphSchemaActions.h"

void UCadenceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewNodeAction> NewNodeAction(
		new FNewNodeAction(
			FText::FromString("Nodes"),
			FText::FromString("New Node"),
			FText::FromString("Makes a new node"),
			0
		));

	ContextMenuBuilder.AddAction(NewNodeAction);
}

const FPinConnectionResponse UCadenceGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if(!A || !B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Null pins!"));

	if(A->Direction == B->Direction)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs!"));
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connection allowed!"));
}

FLinearColor UCadenceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Blue;
}

FLinearColor UCadenceGraphSchema::GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Red;
}
