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
