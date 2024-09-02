// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchemaActions.h"

#include "CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphNodePin.h"
#include "CadenceUserVariableNodes.h"

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UCadenceGraphEditor* ParentEditorGraph = Cast<UCadenceGraphEditor>(ParentGraph);
	ensure(ParentEditorGraph);

	UCadenceGraph* RuntimeGraph = ParentEditorGraph->GetRuntimeGraph();
	ensure(RuntimeGraph);

	RuntimeGraph->Modify();
	UCadenceGraphNode* RuntimeNode = CreateCadenceGraphNode(RuntimeGraph, Location);	
	RuntimeGraph->AddNode(RuntimeNode);
	
	FGraphNodeCreator<UCadenceGraphEditorNode> NodeCreator(*ParentGraph);
	UCadenceGraphEditorNode* Node = NodeCreator.CreateNode(bSelectNewNode);
	Node->Construct(RuntimeNode);
	NodeCreator.Finalize();

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

	ParentGraph->Modify();
	ParentGraph->AddNode(Node, true, true);	
	
	return Node;
}

UCadenceGraphNode* FNewNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	UCadenceGraphNode* Node =  RuntimeGraph->CreateNode(RuntimeNodeType, Location);
	Node->CreateInputPins();
	Node->CreateOutputPins();
	return Node;
}

UCadenceGraphNode* FNewVariableGetterNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	UCadenceGraphNode* Node = RuntimeGraph->CreateNode(UCadenceUserVariableGetterNode::StaticClass(), Location);
	UCadenceUserVariableGetterNode* GetterNode = Cast<UCadenceUserVariableGetterNode>(Node);
	GetterNode->SetSourceVariable(NamedVariable);
	GetterNode->CreateInputPins();
	GetterNode->CreateOutputPins();
	return Node;
}

UCadenceGraphNode* FNewVariableSetterNodeAction::CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location)
{
	UCadenceGraphNode* Node = RuntimeGraph->CreateNode(UCadenceUserVariableSetterNode::StaticClass(), Location);
	UCadenceUserVariableSetterNode* SetterNode = Cast<UCadenceUserVariableSetterNode>(Node);
	SetterNode->SetSourceVariable(NamedVariable);
	SetterNode->CreateInputPins();
	SetterNode->CreateOutputPins();	
	return Node;
}