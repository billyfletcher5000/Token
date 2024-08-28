// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchema.h"

#include "CadenceGraph.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphNode.h"
#include "CadenceGraphNodePin.h"
#include "CadenceGraphSchemaActions.h"
#include "CadenceVariable.h"

const FName UCadenceGraphSchema::PC_Exec = TEXT("exec");
const FName UCadenceGraphSchema::PC_Variable = TEXT("variable");

UCadenceGraphSchema::UCadenceGraphSchema()
{
	GenerateColorMap();
}

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

	UCadenceGraphEditorNode* EditorNodeA = Cast<UCadenceGraphEditorNode>(A->GetOwningNode());
	UCadenceGraphEditorNode* EditorNodeB = Cast<UCadenceGraphEditorNode>(B->GetOwningNode());

	ensure(EditorNodeA);
	ensure(EditorNodeB);

	UCadenceGraphNode* RuntimeNodeA = EditorNodeA->GetRuntimeGraphNode();
	UCadenceGraphNode* RuntimeNodeB = EditorNodeB->GetRuntimeGraphNode();

	ensure(RuntimeNodeA);
	ensure(RuntimeNodeB);
	
	UCadenceGraphNodePin* RuntimePinA = A->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeA->GetInputPin(A->PinName) : RuntimeNodeA->GetOutputPin(A->PinName);
	UCadenceGraphNodePin* RuntimePinB = B->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeB->GetInputPin(B->PinName) : RuntimeNodeB->GetOutputPin(B->PinName);

	ensure(RuntimePinA);
	ensure(RuntimePinB);

	RuntimeNodeA->GetParentGraph()->Modify();

	switch (Response.Response)
	{
	case CONNECT_RESPONSE_MAKE:
		{
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinB);
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_A:
		{
			RuntimePinA->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinB);
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_B:
		{
			RuntimePinB->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinB);
		}
		break;

	case CONNECT_RESPONSE_BREAK_OTHERS_AB:
		{
			RuntimePinA->ClearConnections();
			RuntimePinB->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinB);
		}
		break;
	default:
		break;
	}	
	
	return Super::TryCreateConnection(A, B);
}

FLinearColor UCadenceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_Exec)
		return FLinearColor::Gray;
	
	if(SubCategoryToColor.Contains(PinType.PinSubCategory))
		return SubCategoryToColor[PinType.PinSubCategory];
	
	return FLinearColor::Blue;
}

void UCadenceGraphSchema::GenerateColorMap()
{
	TArray<TObjectPtr<UClass>> ValidCadenceVariableTypes;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceVariable::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			ValidCadenceVariableTypes.Add(Class);
		}
	}

	for(TObjectPtr<UClass> CadenceVariableType : ValidCadenceVariableTypes)
	{
		UCadenceVariable* VariableDefault = CadenceVariableType->GetDefaultObject<UCadenceVariable>();
		SubCategoryToColor.Add(VariableDefault->GetPinSubCategory(), VariableDefault->GetPinColor());
	}
}
