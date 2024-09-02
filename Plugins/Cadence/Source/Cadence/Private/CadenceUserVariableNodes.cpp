// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceUserVariableNodes.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"
#include "CadenceVariable.h"

void UCadenceUserVariableGetterNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
}

bool UCadenceUserVariableGetterNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Value);
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins = OutputPin->GetConnectedPins();
	
	OutputPin->OverrideVariable(SourceVariable);
	
	return true;
}

FText UCadenceUserVariableGetterNode::GetNodeTitle() const
{
	return FText::FromName(SourceVariable->GetUserVariableName());
}

FText UCadenceUserVariableGetterNode::GetNodeMenuName() const
{
	return FCadenceUserVariableHelper::GetGetterNodeMenuName(SourceVariable);
}

void UCadenceUserVariableSetterNode::CreateInputPins()
{
	Super::CreateInputPins();	
	AddInputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
}

void UCadenceUserVariableSetterNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
}

bool UCadenceUserVariableSetterNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Value);
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedInputPins = InputPin->GetConnectedPins();
	
	if(ensure(ConnectedInputPins.Num() == 1))
	{
		UCadenceVariable* InVariable = ConnectedInputPins[0]->GetVariable();
		SourceVariable->CopyValueFrom(InVariable);
	}
	
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Value);
	OutputPin->OverrideVariable(SourceVariable);
	
	return true;
}

FText UCadenceUserVariableSetterNode::GetNodeTitle() const
{
	return FText::FromString(FCadenceUserVariableConstants::SetterPrefix + SourceVariable->GetUserVariableName().ToString());
}

FText UCadenceUserVariableSetterNode::GetNodeMenuName() const
{
	return FCadenceUserVariableHelper::GetSetterNodeMenuName(SourceVariable);
}
