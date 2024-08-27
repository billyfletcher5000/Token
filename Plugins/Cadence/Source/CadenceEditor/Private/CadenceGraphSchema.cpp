// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchema.h"

#include "CadenceGraphNode.h"
#include "CadenceGraphSchemaActions.h"

void UCadenceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TArray<TObjectPtr<UClass>> ValidRuntimeNodeTypes;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceGraphNode::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			ValidRuntimeNodeTypes.Add(Class);
		}
	}

	for(TObjectPtr<UClass> RuntimeNodeType : ValidRuntimeNodeTypes)
	{
		UCadenceGraphNode* DefaultNode = Cast<UCadenceGraphNode>(RuntimeNodeType->GetDefaultObject());
				
		TSharedPtr<FNewNodeAction> NewNodeAction(
			new FNewNodeAction(RuntimeNodeType,
				DefaultNode->GetNodeCategory(),
				DefaultNode->GetNodeMenuName(),
				DefaultNode->GetCreateNodeTooltip(),
				0
			));
		
		ContextMenuBuilder.AddAction(NewNodeAction);
	}
}

const FPinConnectionResponse UCadenceGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if(!A || !B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Null pins!"));

	if(A->Direction == B->Direction)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs!"));
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connection allowed!"));
}

bool UCadenceGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	FPinConnectionResponse Response = CanCreateConnection(A, B);

	switch (Response.Response)
	{
	case CONNECT_RESPONSE_MAKE:
		{
			
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_A:
		{
			
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_B:
		{
			
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_AB:
		{
			
		}
		break;
	default:
		break;
	}
	
	return Super::TryCreateConnection(A, B);
}

FLinearColor UCadenceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Blue;
}

FLinearColor UCadenceGraphSchema::GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::Red;
}
