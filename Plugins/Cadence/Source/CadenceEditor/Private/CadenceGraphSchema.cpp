// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphSchema.h"

#include "CadenceAsset.h"
#include "CadenceEditorCommon.h"
#include "Graph/CadenceGraph.h"
#include "CadenceGraphEditor.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphEditorRerouteNode.h"
#include "CadenceGraphPropertyCustomization.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "CadenceGraphSchemaActions.h"
#include "GraphEditorSettings.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/Nodes/CadenceUserVariableNodes.h"
#include "Graph/CadenceVariable.h"
#include "Graph/Nodes/CadenceRerouteNodes.h"

const FName UCadenceGraphSchema::PC_Exec = TEXT("exec");
const FName UCadenceGraphSchema::PC_Variable = TEXT("variable");
const FName UCadenceGraphSchema::PC_Int = TEXT("int");
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

	if (A->PinType.ContainerType != B->PinType.ContainerType)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect a pin of a different container type (e.g. variable to array)!"));

	const FName PinACategory = A->PinType.PinCategory;
	const FName PinBCategory = B->PinType.PinCategory;
	
	if (PinACategory != PinBCategory)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be of same type or convertible!"));

	if (IsVariablePinCategory(PinACategory) || IsVariablePinCategory(PinBCategory))
	{
		const UCadenceGraphEditorNode* CadenceEditorNodeA = Cast<UCadenceGraphEditorNode>(A->GetOwningNode());
		const UCadenceGraphEditorNode* CadenceEditorNodeB = Cast<UCadenceGraphEditorNode>(B->GetOwningNode());

		const UCadenceGraphNode* RuntimeNodeA = CadenceEditorNodeA->GetRuntimeGraphNode();
		const UCadenceGraphNode* RuntimeNodeB = CadenceEditorNodeB->GetRuntimeGraphNode();

		UCadenceGraphNodePin* RuntimePinA = A->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeA->GetInputPin(A->PinName) : RuntimeNodeA->GetOutputPin(A->PinName);
		UCadenceGraphNodePin* RuntimePinB = B->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeB->GetInputPin(B->PinName) : RuntimeNodeB->GetOutputPin(B->PinName);

		if(RuntimePinA && RuntimePinB && !RuntimePinA->GetVariableClass()->IsChildOf(RuntimePinB->GetVariableClass()) && !RuntimePinB->GetVariableClass()->IsChildOf(RuntimePinA->GetVariableClass()))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be of same type or convertible!"));
		}
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connection allowed!"));
}

bool UCadenceGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const FScopedTransaction Transaction(*FCadenceEditorCommon::ContextIdentifier, FText::FromString(TEXT("Try Create Connection")), nullptr);
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
	
	if(UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(RuntimeNodeA))
	{		
		if(RuntimePinB->IsExec())
			RerouteNode->SetAsExecReroute();
		else
			RerouteNode->SetVariableType(RuntimePinB->GetVariableClass());

		RuntimePinA = A->Direction == EEdGraphPinDirection::EGPD_Input ? RuntimeNodeA->GetInputPin(A->PinName) : RuntimeNodeA->GetOutputPin(A->PinName);
	}

	if(UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(RuntimeNodeB))
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
	const FScopedTransaction Transaction(*FCadenceEditorCommon::ContextIdentifier, FText::FromString(TEXT("Break Node Links")), nullptr);
	if(UCadenceGraphEditorNode* CadenceGraphEditorNode = Cast<UCadenceGraphEditorNode>(&TargetNode); CadenceGraphEditorNode != nullptr)
	{
		CadenceGraphEditorNode->GetRuntimeGraphNode()->Modify();
		CadenceGraphEditorNode->GetRuntimeGraphNode()->ClearConnections();
	}
	
	Super::BreakNodeLinks(TargetNode);
}

void UCadenceGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(*FCadenceEditorCommon::ContextIdentifier, FText::FromString(TEXT("Break Pin Links")), nullptr);
	
	if(UCadenceGraphEditorNode* CadenceGraphEditorNode = Cast<UCadenceGraphEditorNode>(TargetPin.GetOwningNode()); CadenceGraphEditorNode != nullptr)
	{
		UCadenceGraphNodePin* RuntimePin = nullptr;
		if(TargetPin.Direction == EGPD_Input)
			RuntimePin = CadenceGraphEditorNode->GetRuntimeGraphNode()->GetInputPin(TargetPin.PinName);
		else
			RuntimePin = CadenceGraphEditorNode->GetRuntimeGraphNode()->GetOutputPin(TargetPin.PinName);

		ensure(RuntimePin);
		RuntimePin->Modify();
		RuntimePin->ClearConnections();
		
		if(UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(RuntimePin->GetParentNode()))
		{
			RerouteNode->Modify();
			RerouteNode->CheckRerouteTypeValid();
		}
	}
	
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UCadenceGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{	
	const FScopedTransaction Transaction(*FCadenceEditorCommon::ContextIdentifier, FText::FromString(TEXT("Break Single Pin Link")), nullptr);
	
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

		if(UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(RuntimeSourcePin->GetParentNode()))
			RerouteNode->CheckRerouteTypeValid();

		if(UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(RuntimeTargetPin->GetParentNode()))
			RerouteNode->CheckRerouteTypeValid();
	}
	
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

void UCadenceGraphSchema::OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const
{	
	const FScopedTransaction Transaction(*FCadenceEditorCommon::ContextIdentifier, FText::FromString("Create Reroute Node"), nullptr);

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

void UCadenceGraphSchema::TrySetDefaultValue(UEdGraphPin& Pin, const FString& NewDefaultValue, bool bMarkAsModified) const
{
	UCadenceGraphEditorNode* ParentEditorNode = Cast<UCadenceGraphEditorNode>(Pin.GetOwningNode());
	check(ParentEditorNode);

	UCadenceGraphNode* ParentNode = ParentEditorNode->GetRuntimeGraphNode();
	UCadenceGraphNodePin* RuntimePin = Pin.Direction == EEdGraphPinDirection::EGPD_Input ? ParentNode->GetInputPin(Pin.PinName) : ParentNode->GetOutputPin(Pin.PinName);

	check(RuntimePin);
	UCadenceVariable* Variable = RuntimePin->GetVariable();
	
	if(Variable && Variable->SupportsDefault())
	{
		Variable->Modify();
		Variable->SetFromString(NewDefaultValue);
	}

	// This has to be done last as the variable default value changes get propagated to the editor nodes/pins as a result of the Super
	Super::TrySetDefaultValue(Pin, NewDefaultValue, bMarkAsModified);
}

FLinearColor UCadenceGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinCategoryToColor.Contains(PinType.PinCategory))
		return PinCategoryToColor[PinType.PinCategory];
	
	return FLinearColor::White;
}

FLinearColor UCadenceGraphSchema::GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.IsMap())
	{
		FEdGraphPinType FakePrimary = PinType;
		FakePrimary.PinCategory = FakePrimary.PinValueType.TerminalCategory;
		FakePrimary.PinSubCategory = FakePrimary.PinValueType.TerminalSubCategory;
		FakePrimary.PinSubCategoryObject = FakePrimary.PinValueType.TerminalSubCategoryObject;

		return GetPinTypeColor(FakePrimary);
	}
	else
	{
		const UGraphEditorSettings* Settings = GetDefault<UGraphEditorSettings>();
		return Settings->WildcardPinTypeColor;
	}
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

bool UCadenceGraphSchema::ConvertVariableToPinType(const UCadenceVariable* InVariable, FEdGraphPinType& OutPinType) const
{
	if(!IsValid(InVariable))
		return false;

	OutPinType.PinCategory = InVariable->GetPinCategory();
	OutPinType.PinSubCategory = InVariable->GetPinSubCategory();
	OutPinType.PinSubCategoryObject = InVariable->GetPinSubCategoryObject();
	OutPinType.ContainerType = InVariable->IsArray() ? EPinContainerType::Array : EPinContainerType::None;

	return true;
}

UCadenceUserVariableGetterNode* UCadenceGraphSchema::SpawnVariableGetNode(const FVector2D InGraphPosition, UCadenceGraph* InGraph, UCadenceVariable* InVariable) const
{
	UCadenceUserVariableGetterNode* Node = FNewVariableGetterNodeAction::CreateGetterNode(InGraph, InVariable, InGraphPosition);
	return Node;
}

UCadenceUserVariableSetterNode* UCadenceGraphSchema::SpawnVariableSetNode(const FVector2D InGraphPosition, UCadenceGraph* InGraph, UCadenceVariable* InVariable) const
{
	UCadenceUserVariableSetterNode* Node = FNewVariableSetterNodeAction::CreateSetterNode(InGraph, InVariable, InGraphPosition);
	return Node;
}

bool UCadenceGraphSchema::IsExecPin(const UEdGraphPin& EdGraphPin) const
{
	return EdGraphPin.PinType.PinCategory == PC_Exec;
}

bool UCadenceGraphSchema::ArePinTypesCompatible(const FEdGraphPinType& InPinTypeA, const FEdGraphPinType& InPinTypeB) const
{
	return InPinTypeA.PinCategory == InPinTypeB.PinCategory && InPinTypeA.PinSubCategory == InPinTypeB.PinSubCategory && InPinTypeA.ContainerType == InPinTypeB.ContainerType;
}

void UCadenceGraphSchema::GetVariableTypeTree(TArray<FPinTypeTreeItem>& OutTypeTreeArray, ETypeTreeFilter InTreeFilter) const
{
	TArray<TObjectPtr<UClass>> ValidVariableTypes;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceVariable::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			ValidVariableTypes.Add(Class);
		}
	}

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	for(TObjectPtr<UClass> VariableType : ValidVariableTypes)
	{
		UCadenceVariable* VariableCDO = VariableType->GetDefaultObject<UCadenceVariable>();
		FName VariableDisplayName = VariableCDO->GetDisplayName();		
		FName VariableCategory = VariableCDO->GetPinCategory();

		if(VariableDisplayName == NAME_None)
			VariableDisplayName = VariableCategory;
		
		OutTypeTreeArray.Add( MakeShareable( new UEdGraphSchema_K2::FPinTypeTreeInfo(FText::FromName(VariableDisplayName), VariableCategory, K2Schema, FText::FromName(VariableDisplayName)) ) );
	}
}

bool UCadenceGraphSchema::IsVariablePinCategory(const FName& InPinCategory)
{
	return InPinCategory != PC_Exec;
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

	PinCategoryToColor.Add(FCadencePinCategoryConstants::PC_Exec, FLinearColor::Gray);
	PinCategoryToColor.Add(FCadencePinCategoryConstants::PC_Wildcard, FLinearColor(0.5f, 0.5f, 0.5f));

	for(TObjectPtr<UClass> CadenceVariableType : ValidCadenceVariableTypes)
	{
		UCadenceVariable* VariableDefault = CadenceVariableType->GetDefaultObject<UCadenceVariable>();
		if(!PinCategoryToColor.Contains(VariableDefault->GetPinCategory()))
			PinCategoryToColor.Add(VariableDefault->GetPinCategory(), VariableDefault->GetPinColor());
	}
}
