// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "Graph/CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorGridNode.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphPropertyCustomization.h"
#include "CadenceGraphSchema.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/Nodes/CadenceGridNodes.h"
#include "Graph/Nodes/CadenceOperationNodes.h"
#include "Graph/Nodes/CadenceUserVariableNodes.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	FScopedTransaction Transaction(FText::FromString("Cadence: New Node"));
	UCadenceGraphEditor* ParentEditorGraph = Cast<UCadenceGraphEditor>(ParentGraph);
	ensure(ParentEditorGraph);

	ParentGraph->Modify();

	UCadenceGraph* RuntimeGraph = ParentEditorGraph->GetRuntimeGraph();
	ensure(RuntimeGraph);

	RuntimeGraph->Modify();
	UCadenceGraphNode* RuntimeNode = CreateCadenceGraphNode(RuntimeGraph, Location);	
	RuntimeGraph->AddNode(RuntimeNode);
		
	UCadenceGraphEditorNode* Node = nullptr;

	if(RuntimeNode->GetClass()->ImplementsInterface(UCadenceGraphGridCommandProvider::StaticClass()))
	{
		FGraphNodeCreator<UCadenceGraphEditorGridNode> NodeCreator(*ParentGraph);
		Node = NodeCreator.CreateNode(false);
		Node->Construct(RuntimeNode);
		NodeCreator.Finalize();
	}
	else
	{
		FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*ParentGraph);
		Node = NodeCreator.CreateNode(false);
		Node->Construct(RuntimeNode);
		NodeCreator.Finalize();
	}

	if(UCadenceOperationNode_Base* OperationNode = Cast<UCadenceOperationNode_Base>(RuntimeNode))
	{
		UE_LOG(LogCadenceEditor, Log, TEXT("FNewNodeAction::PerformAction UpdateOperationNode"));
		const UCadenceGraphSchema* CadenceGraphSchema = Cast<UCadenceGraphSchema>(ParentEditorGraph->GetSchema());
		CadenceGraphSchema->UpdateOperationNode(OperationNode);
	}

	if (FromPin)
	{
		UCadenceGraphEditorNode* FromEditorNode = Cast<UCadenceGraphEditorNode>(FromPin->GetOwningNode());
		UCadenceGraphNode* FromRuntimeNode = FromEditorNode->GetRuntimeGraphNode();
		if(FromPin->Direction == EGPD_Input)
		{
			if(UCadenceGraphNodePin* FromRuntimePin = FromRuntimeNode->GetInputPin(FromPin->PinName))
			{
				if(UCadenceGraphNodePin* RuntimeNodePin = RuntimeNode->GetMostAppropriateAutomaticOutputPin(FromRuntimePin))
				{
					UEdGraphPin* AppropriateEdPin = Node->GetOutputPinByName(RuntimeNodePin->GetPinName());
					ParentEditorGraph->GetSchema()->TryCreateConnection(AppropriateEdPin, FromPin);
				}
			}
		}
		else
		{
			if(UCadenceGraphNodePin* FromRuntimePin = FromRuntimeNode->GetOutputPin(FromPin->PinName))
			{
				if(UCadenceGraphNodePin* RuntimeNodePin = RuntimeNode->GetMostAppropriateAutomaticInputPin(FromRuntimePin))
				{
					UEdGraphPin* AppropriateEdPin = Node->GetInputPinByName(RuntimeNodePin->GetPinName());
					ParentEditorGraph->GetSchema()->TryCreateConnection(AppropriateEdPin, FromPin);
				}
			}
		}
	}
	ParentGraph->NotifyGraphChanged();
	
	return Node;
}

UCadenceGraphNode* FNewNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	UCadenceGraphNode* Node = RuntimeGraph->CreateNode(RuntimeNodeType, Location);
	Node->Modify();
	Node->CreateInputPins();
	Node->CreateOutputPins();
	return Node;
}

UCadenceUserVariableGetterNode* FNewVariableGetterNodeAction::CreateGetterNode(UCadenceGraph* InGraph, UCadenceVariable* InVariable, const FVector2D& InLocation)
{
	UCadenceGraphNode* Node = InGraph->CreateNode(UCadenceUserVariableGetterNode::StaticClass(), InLocation);
	UCadenceUserVariableGetterNode* GetterNode = Cast<UCadenceUserVariableGetterNode>(Node);
	GetterNode->Modify();
	GetterNode->SetSourceVariable(InVariable);
	GetterNode->CreateInputPins();
	GetterNode->CreateOutputPins();
	return GetterNode;
}

UCadenceGraphNode* FNewVariableGetterNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	return CreateGetterNode(RuntimeGraph, NamedVariable.Variable, Location);
}

UCadenceUserVariableSetterNode* FNewVariableSetterNodeAction::CreateSetterNode(UCadenceGraph* InGraph, UCadenceVariable* InVariable, const FVector2D& InLocation)
{	
	UCadenceGraphNode* Node = InGraph->CreateNode(UCadenceUserVariableSetterNode::StaticClass(), InLocation);
	UCadenceUserVariableSetterNode* SetterNode = Cast<UCadenceUserVariableSetterNode>(Node);
	SetterNode->Modify();
	SetterNode->SetSourceVariable(InVariable);
	SetterNode->CreateInputPins();
	SetterNode->CreateOutputPins();
	
	return SetterNode;
}

UCadenceGraphNode* FNewVariableSetterNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	return CreateSetterNode(RuntimeGraph, NamedVariable.Variable, Location);
}

FEdGraphPinType FCadenceVariableAction::GetPinType() const
{
	const UCadenceGraphSchema* Schema = GetDefault<UCadenceGraphSchema>();
	FEdGraphPinType OutType;
	Schema->ConvertVariableToPinType(Variable.Get(), OutType);
	return OutType;
}

bool FCadenceVariableAction::IsValidName(const FName& NewName, FText& OutErrorMessage) const
{
	return true;
}

void FCadenceVariableAction::DeleteVariable()
{
	// TODO
}

bool FCadenceVariableAction::IsVariableUsed()
{
	// TODO
	return false;
}

void FCadenceVariableAction::MovePersistentItemToCategory(const FText& NewCategoryName)
{
	if(Variable.IsValid())
		Variable->SetCategory(NewCategoryName);
}

int32 FCadenceVariableAction::GetReorderIndexInContainer() const
{
	return FEdGraphSchemaAction::GetReorderIndexInContainer();
}

bool FCadenceVariableAction::ReorderToBeforeAction(TSharedRef<FEdGraphSchemaAction> OtherAction)
{
	return FEdGraphSchemaAction::ReorderToBeforeAction(OtherAction);
}

FEdGraphSchemaActionDefiningObject FCadenceVariableAction::GetPersistentItemDefiningObject() const
{
	return FEdGraphSchemaActionDefiningObject(Variable.Get());
}

void FCadenceGraphAction::MovePersistentItemToCategory(const FText& NewCategoryName)
{
	FEdGraphSchemaAction::MovePersistentItemToCategory(NewCategoryName);
}

int32 FCadenceGraphAction::GetReorderIndexInContainer() const
{
	return INDEX_NONE;
}

bool FCadenceGraphAction::ReorderToBeforeAction(TSharedRef<FEdGraphSchemaAction> OtherAction)
{
	return FEdGraphSchemaAction::ReorderToBeforeAction(OtherAction);
}

FEdGraphSchemaActionDefiningObject FCadenceGraphAction::GetPersistentItemDefiningObject() const
{
	return FEdGraphSchemaAction::GetPersistentItemDefiningObject();
}