// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchema.h"

#include "Graph/CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphEditorRerouteNode.h"
#include "CadenceGraphPropertyCustomization.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "CadenceGraphSchemaActions.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/Nodes/CadenceUserVariableNodes.h"
#include "Graph/CadenceVariable.h"
#include "Graph/Nodes/CadenceRerouteNodes.h"

const FName UCadenceGraphSchema::PC_Exec = TEXT("exec");
const FName UCadenceGraphSchema::PC_Variable = TEXT("variable");
const FName UCadenceGraphSchema::PC_Wildcard = TEXT("wildcard");

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
		if(DefaultNode->CanBeAutoCreated())
		{
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

	const UCadenceGraphEditor* CadenceGraphEditor = Cast<UCadenceGraphEditor>(ContextMenuBuilder.CurrentGraph);
	UCadenceGraph* RuntimeGraph = CadenceGraphEditor->GetRuntimeGraph();
	FCadenceGraphUserVariableSet& UserVariables = RuntimeGraph->UserVariables;
	for(FCadenceNamedVariable& NamedVariable : UserVariables.Variables)
	{		
		UCadenceGraphNode* DefaultNode = Cast<UCadenceGraphNode>(UCadenceUserVariableGetterNode::StaticClass()->GetDefaultObject());
		TSharedPtr<FNewVariableSetterNodeAction> SetterAction(
				new FNewVariableSetterNodeAction(NamedVariable,
					DefaultNode->GetNodeCategory(),
					FCadenceUserVariableHelper::GetSetterNodeMenuName(NamedVariable.Variable),
					DefaultNode->GetCreateNodeTooltip(),
					0
				));

		TSharedPtr<FNewVariableGetterNodeAction> GetterAction(
				new FNewVariableGetterNodeAction(NamedVariable,
					DefaultNode->GetNodeCategory(),
					FCadenceUserVariableHelper::GetGetterNodeMenuName(NamedVariable.Variable),
					DefaultNode->GetCreateNodeTooltip(),
					0
				));
		
		ContextMenuBuilder.AddAction(SetterAction);
		ContextMenuBuilder.AddAction(GetterAction);
	}
}

const FPinConnectionResponse UCadenceGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Null pins!"));

	if (A == B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect a pin to itself!"));

	if (A->GetOwningNode() == B->GetOwningNode())
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect a node to itself!"));

	if (A->Direction == B->Direction)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs!"));

	const FName PinACategory = A->PinType.PinCategory;
	const FName PinBCategory = B->PinType.PinCategory;
	
	if (PinACategory != PinBCategory)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be of same type or convertible!"));

	if (PinACategory == PC_Variable || PinBCategory == PC_Variable)
	{
		const UCadenceGraphEditorNode* CadenceEditorNodeA = Cast<UCadenceGraphEditorNode>(A->GetOwningNode());
		const UCadenceGraphEditorNode* CadenceEditorNodeB = Cast<UCadenceGraphEditorNode>(B->GetOwningNode());

		const UCadenceGraphNode* RuntimeNodeA = CadenceEditorNodeA->GetRuntimeGraphNode();
		const UCadenceGraphNode* RuntimeNodeB = CadenceEditorNodeB->GetRuntimeGraphNode();

		UCadenceGraphNodePin* RuntimePinA = A->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeA->GetInputPin(A->PinName) : RuntimeNodeA->GetOutputPin(A->PinName);
		UCadenceGraphNodePin* RuntimePinB = B->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeB->GetInputPin(B->PinName) : RuntimeNodeB->GetOutputPin(B->PinName);

		if(RuntimePinA && RuntimePinB && RuntimePinA->GetVariableClass() != RuntimePinB->GetVariableClass())
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be of same type or convertible!"));
		}
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connection allowed!"));
}

bool UCadenceGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	FPinConnectionResponse Response = CanCreateConnection(A, B);

	if(Response.Response == CONNECT_RESPONSE_DISALLOW)
		return false;

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
	
	if(UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimeNodeA))
	{		
		if(RuntimePinB->IsExec())
			RerouteNode->SetAsExecReroute();
		else
			RerouteNode->SetVariableType(RuntimePinB->GetVariableClass());

		RuntimePinA = A->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeA->GetInputPin(A->PinName) : RuntimeNodeA->GetOutputPin(A->PinName);
	}

	if(UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimeNodeB))
	{		
		if(RuntimePinA->IsExec())
			RerouteNode->SetAsExecReroute();
		else
			RerouteNode->SetVariableType(RuntimePinA->GetVariableClass());

		RuntimePinB = B->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeB->GetInputPin(B->PinName) : RuntimeNodeB->GetOutputPin(B->PinName);
	}	

	ensure(RuntimePinA);
	ensure(RuntimePinB);

	RuntimeNodeA->GetParentGraph()->Modify();

	switch (Response.Response)
	{
		case CONNECT_RESPONSE_MAKE:		
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinA);		
			break;

		case CONNECT_RESPONSE_BREAK_OTHERS_A:		
			RuntimePinA->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinA);		
			break;

		case CONNECT_RESPONSE_BREAK_OTHERS_B:		
			RuntimePinB->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinA);		
			break;

		case CONNECT_RESPONSE_BREAK_OTHERS_AB:		
			RuntimePinA->ClearConnections();
			RuntimePinB->ClearConnections();
			RuntimePinA->ConnectPin(RuntimePinB);
			RuntimePinB->ConnectPin(RuntimePinA);		
			break;
			
		default:
			break;
	}	
	
	return Super::TryCreateConnection(A, B);
}

void UCadenceGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	if(UCadenceGraphEditorNode* CadenceGraphEditorNode = Cast<UCadenceGraphEditorNode>(&TargetNode); CadenceGraphEditorNode != nullptr)
	{
		CadenceGraphEditorNode->GetRuntimeGraphNode()->ClearConnections();
	}
	
	Super::BreakNodeLinks(TargetNode);
}

void UCadenceGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	if(UCadenceGraphEditorNode* CadenceGraphEditorNode = Cast<UCadenceGraphEditorNode>(TargetPin.GetOwningNode()); CadenceGraphEditorNode != nullptr)
	{
		UCadenceGraphNodePin* RuntimePin = nullptr;
		if(TargetPin.Direction == EGPD_Input)
			RuntimePin = CadenceGraphEditorNode->GetRuntimeGraphNode()->GetInputPin(TargetPin.PinName);
		else
			RuntimePin = CadenceGraphEditorNode->GetRuntimeGraphNode()->GetOutputPin(TargetPin.PinName);

		ensure(RuntimePin);
		RuntimePin->ClearConnections();
		
		if(UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimePin->GetParentNode()))
			RerouteNode->CheckRerouteTypeValid();
	}
	
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UCadenceGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UCadenceGraphEditorNode* SourceEditorNode = Cast<UCadenceGraphEditorNode>(SourcePin->GetOwningNode());
	UCadenceGraphEditorNode* TargetEditorNode = Cast<UCadenceGraphEditorNode>(TargetPin->GetOwningNode());
	if(SourceEditorNode != nullptr && TargetEditorNode != nullptr)
	{
		UCadenceGraphNodePin* RuntimeSourcePin = nullptr;
		if(SourcePin->Direction == EGPD_Input)
			RuntimeSourcePin = SourceEditorNode->GetRuntimeGraphNode()->GetInputPin(SourcePin->PinName);
		else
			RuntimeSourcePin = SourceEditorNode->GetRuntimeGraphNode()->GetOutputPin(SourcePin->PinName);

		UCadenceGraphNodePin* RuntimeTargetPin = nullptr;
		if(TargetPin->Direction == EGPD_Input)
			RuntimeTargetPin = TargetEditorNode->GetRuntimeGraphNode()->GetInputPin(TargetPin->PinName);
		else
			RuntimeTargetPin = TargetEditorNode->GetRuntimeGraphNode()->GetOutputPin(TargetPin->PinName);

		ensure(RuntimeSourcePin);
		ensure(RuntimeTargetPin);

		RuntimeSourcePin->DisconnectPin(RuntimeTargetPin);
		RuntimeTargetPin->DisconnectPin(RuntimeSourcePin);

		if(UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimeSourcePin->GetParentNode()))
			RerouteNode->CheckRerouteTypeValid();

		if(UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimeTargetPin->GetParentNode()))
			RerouteNode->CheckRerouteTypeValid();
	}
	
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

void UCadenceGraphSchema::OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const
{	
	const FScopedTransaction Transaction(FText::FromString("Create Reroute Node"));

	//@TODO: This constant is duplicated from inside of SGraphNodeKnot
	const FVector2D NodeSpacerSize(42.0f, 24.0f);
	const FVector2D KnotTopLeft = GraphPosition - (NodeSpacerSize * 0.5f);

	UEdGraphPin* InputEdPin = PinA->Direction == EEdGraphPinDirection::EGPD_Output ? PinA : PinB;
	UEdGraphPin* OutputEdPin = PinA->Direction == EEdGraphPinDirection::EGPD_Input ? PinA : PinB;

	UCadenceGraphEditorNode* InputEditorNode = Cast<UCadenceGraphEditorNode>(InputEdPin->GetOwningNode());
	UCadenceGraphEditorNode* OutputEditorNode = Cast<UCadenceGraphEditorNode>(OutputEdPin->GetOwningNode());

	UCadenceGraphEditor* EditorGraph = Cast<UCadenceGraphEditor>(InputEditorNode->GetGraph());

	UCadenceGraphNode* InputRuntimeNode = InputEditorNode->GetRuntimeGraphNode();
	UCadenceGraphNode* OutputRuntimeNode = OutputEditorNode->GetRuntimeGraphNode();

	UCadenceGraphNodePin* InputRuntimePin = InputRuntimeNode->GetOutputPin(InputEdPin->PinName);
	UCadenceGraphNodePin* OutputRuntimePin = OutputRuntimeNode->GetInputPin(OutputEdPin->PinName);
	
	UCadenceGraph* RuntimeGraph = InputRuntimeNode->GetParentGraph();

	UCadenceSimpleRerouteNode* RerouteNode = Cast<UCadenceSimpleRerouteNode>(RuntimeGraph->CreateNode(UCadenceSimpleRerouteNode::StaticClass(), KnotTopLeft));
	if(InputRuntimePin->IsExec() && OutputRuntimePin->IsExec())
	{
		RerouteNode->SetAsExecReroute();
	}
	else if(InputRuntimePin->GetVariableClass() == OutputRuntimePin->GetVariableClass())
	{
		RerouteNode->SetVariableType(InputRuntimePin->GetVariableClass());
	}
	else
	{
		UE_LOG(LogCadenceEditor, Error, TEXT("Attempt to create reroute node using an already invalid connection!"));
		RuntimeGraph->RemoveNode(RerouteNode);
		return;
	}

	
	RuntimeGraph->Modify();
	RuntimeGraph->AddNode(RerouteNode);

	BreakSinglePinLink(PinA, PinB);
	
	FGraphNodeCreator<UCadenceGraphEditorRerouteNode> NodeCreator(*InputEditorNode->GetGraph());
	UCadenceGraphEditorRerouteNode* Node = NodeCreator.CreateNode(true);
	Node->Construct(RerouteNode);
	NodeCreator.Finalize();

	EditorGraph->AddNode(Node, true, true);	

	UEdGraphPin* RerouteInputEdPin = Node->GetInputPinByName(RerouteNode->GetRerouteInputPin()->GetPinName());
	UEdGraphPin* RerouteOutputEdPin = Node->GetOutputPinByName(RerouteNode->GetRerouteOutputPin()->GetPinName());
	
	TryCreateConnection(InputEdPin, RerouteInputEdPin);
	TryCreateConnection(RerouteOutputEdPin, OutputEdPin);		
}

FLinearColor UCadenceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_Exec)
		return FLinearColor::Gray;
	
	if (SubCategoryToColor.Contains(PinType.PinSubCategory))
		return SubCategoryToColor[PinType.PinSubCategory];
	
	return FLinearColor::Gray;
}

FText UCadenceGraphSchema::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	if(Pin->PinName == FCadencePinConstants::Pin_Default_Exec ||
	   Pin->PinName == FCadencePinConstants::Pin_Default_Then)
	{
		return FText::FromString(TEXT_EMPTY);
	}
		
	return Super::GetPinDisplayName(Pin);
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
