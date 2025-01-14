// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceComparisonNodes.h"

#include "Graph/CadenceVariable.h"

namespace FCadenceComparisonNodeConstants
{
	static const TSet<TSubclassOf<UCadenceVariable>> AllowedVariableTypesNumeric =
		{
		UCadenceVariableInt::StaticClass(),
		UCadenceVariableFloat::StaticClass(),
		UCadenceVariableDouble::StaticClass(),
		};
}

void UCadenceComparisonNode_Base::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardPin(FCadenceComparisonNodeConstants::Pin_A, GetAllowedWildcardTypes());
	AddInputVariableWildcardPin(FCadenceComparisonNodeConstants::Pin_B, GetAllowedWildcardTypes());
}

void UCadenceComparisonNode_Base::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadenceComparisonNodeConstants::Pin_Result, UCadenceVariableBool::StaticClass());
}

ECadenceNodeExecuteResult UCadenceComparisonNode_Base::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* PinA = GetInputPin(FCadenceComparisonNodeConstants::Pin_A);
	UCadenceGraphNodePin* PinB = GetInputPin(FCadenceComparisonNodeConstants::Pin_B);

	//TODO: Find a nicer way of doing this
	bool ValueFound = false;
	bool Result = false;
	
	if(ProcessComparison<UCadenceVariableInt>(PinA, PinB, Result))
		ValueFound = true;
	else if(ProcessComparison<UCadenceVariableFloat>(PinA, PinB, Result))
		ValueFound = true;
	else if(ProcessComparison<UCadenceVariableDouble>(PinA, PinB, Result))
		ValueFound = true;
	else if(ProcessComparison<UCadenceVariableBool>(PinA, PinB, Result))
		ValueFound = true;

	if(!ValueFound)
		return ECadenceNodeExecuteResult::Failed;
	
	SetOutputPinValue<UCadenceVariableBool, bool>(FCadenceComparisonNodeConstants::Pin_Result, Result);
	
	return ECadenceNodeExecuteResult::Complete;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonEqualsNode::GetAllowedWildcardTypes() const
{
	return {};
}

ECadenceNodeExecuteResult UCadenceComparisonEqualsNode::Execute(UCadenceContext* InContext)
{
	// Equals and Not Equals are special cases as each Variable type has already implemented Equals therefore we can have it apply to all types
	UCadenceGraphNodePin* PinA = GetInputPin(FCadenceComparisonNodeConstants::Pin_A);
	UCadenceGraphNodePin* PinB = GetInputPin(FCadenceComparisonNodeConstants::Pin_B);

	UCadenceVariable* VariableA = PinA->GetVariable();
	UCadenceVariable* VariableB = PinB->GetVariable();

	SetOutputPinValue<UCadenceVariableBool, bool>(FCadenceComparisonNodeConstants::Pin_Result, VariableA->Equals(VariableB));
	
	return ECadenceNodeExecuteResult::Complete;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonNotEqualsNode::GetAllowedWildcardTypes() const
{
	return {};
}

ECadenceNodeExecuteResult UCadenceComparisonNotEqualsNode::Execute(UCadenceContext* InContext)
{
	// Equals and Not Equals are special cases as each Variable type has already implemented Equals therefore we can have it apply to all types
	UCadenceGraphNodePin* PinA = GetInputPin(FCadenceComparisonNodeConstants::Pin_A);
	UCadenceGraphNodePin* PinB = GetInputPin(FCadenceComparisonNodeConstants::Pin_B);

	UCadenceVariable* VariableA = PinA->GetVariable();
	UCadenceVariable* VariableB = PinB->GetVariable();

	SetOutputPinValue<UCadenceVariableBool, bool>(FCadenceComparisonNodeConstants::Pin_Result, !VariableA->Equals(VariableB));
	
	return ECadenceNodeExecuteResult::Complete;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonGreaterThanNode::GetAllowedWildcardTypes() const
{
	return FCadenceComparisonNodeConstants::AllowedVariableTypesNumeric;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonGreaterThanOrEqualsNode::GetAllowedWildcardTypes() const
{
	return FCadenceComparisonNodeConstants::AllowedVariableTypesNumeric;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonLessThanNode::GetAllowedWildcardTypes() const
{
	return FCadenceComparisonNodeConstants::AllowedVariableTypesNumeric;
}

TSet<TSubclassOf<UCadenceVariable>> UCadenceComparisonLessThanOrEqualsNode::GetAllowedWildcardTypes() const
{
	return FCadenceComparisonNodeConstants::AllowedVariableTypesNumeric;
}
