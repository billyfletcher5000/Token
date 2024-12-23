// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceUserVariableNodes.h"

#include "Cadence.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceUserVariableGetterNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	UCadenceGraphNodePin* Pin = AddOutputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
	
	if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(SourceVariable))
	{		
		UCadenceVariableArray* NewArrayVariable = UCadenceVariableArray::Create(ArrayVariable->GetVariableClass(), Pin);
		Pin->SetVariable(NewArrayVariable);
	}
}

ECadenceNodeExecuteResult UCadenceUserVariableGetterNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Value);
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins = OutputPin->GetConnectedPins();
	
	OutputPin->OverrideVariable(SourceVariable);
	
	return ECadenceNodeExecuteResult::Complete;
}

UCadenceGraphNodePin* UCadenceUserVariableGetterNode::GetVariableOutputPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Value);
}

FText UCadenceUserVariableGetterNode::GetNodeTitle() const
{
	return FText::FromName(SourceVariable->GetUserVariableName());
}

FText UCadenceUserVariableGetterNode::GetNodeMenuName() const
{
	return FCadenceUserVariableHelper::GetGetterNodeMenuName(SourceVariable);
}

void UCadenceUserVariableGetterNode::SetSourceVariable(UCadenceVariable* InVariable)
{
	if(SourceVariable != InVariable)
	{
		Super::SetSourceVariable(InVariable);
		
		if(UCadenceGraphNodePin* Pin = GetVariableOutputPin())
		{
			Pin->ClearConnections();
			Pin->SetVariableClass(InVariable->GetClass());
			if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(InVariable))
			{
				Pin->GetVariable<UCadenceVariableArray>()->SetVariableClass(ArrayVariable->GetVariableClass());
			}
		}
	}
}

void UCadenceUserVariableSetterNode::CreateInputPins()
{
	Super::CreateInputPins();
	
	UCadenceGraphNodePin* Pin = AddInputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
	
	if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(SourceVariable))
	{		
		UCadenceVariableArray* NewArrayVariable = UCadenceVariableArray::Create(ArrayVariable->GetVariableClass(), Pin);
		Pin->SetVariable(NewArrayVariable);
	}
}

void UCadenceUserVariableSetterNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	UCadenceGraphNodePin* Pin = AddOutputVariablePin(FCadencePinConstants::Pin_Value, SourceVariable->GetClass());
	
	if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(SourceVariable))
	{		
		UCadenceVariableArray* NewArrayVariable = UCadenceVariableArray::Create(ArrayVariable->GetVariableClass(), Pin);
		Pin->SetVariable(NewArrayVariable);
	}
}

UCadenceGraphNodePin* UCadenceUserVariableSetterNode::GetVariableInputPin() const
{
	return GetInputPin(FCadencePinConstants::Pin_Value);
}

UCadenceGraphNodePin* UCadenceUserVariableSetterNode::GetVariableOutputPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Value);
}

ECadenceNodeExecuteResult UCadenceUserVariableSetterNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Value);
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedInputPins = InputPin->GetConnectedPins();
	
	if(ensure(ConnectedInputPins.Num() == 1))
	{
		UCadenceVariable* InVariable = ConnectedInputPins[0]->GetVariable();
		UE_LOG(LogCadence, Log, TEXT("UCadenceUserVariableSetterNode::Execute Pre-CopyValue: %s - %s"), *GetDebugName(), *ConnectedInputPins[0]->GetGUID().ToString());
		InContext->ParentNode = this;
		SourceVariable->CopyValueFrom(InVariable, InContext);
	}
	else
	{
		return ECadenceNodeExecuteResult::Failed;
	}
	
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Value);
	OutputPin->OverrideVariable(SourceVariable);
	
	return ECadenceNodeExecuteResult::Complete;
}

FText UCadenceUserVariableSetterNode::GetNodeTitle() const
{
	return FText::FromString(FCadenceUserVariableConstants::SetterPrefix + SourceVariable->GetUserVariableName().ToString());
}

FText UCadenceUserVariableSetterNode::GetNodeMenuName() const
{
	return FCadenceUserVariableHelper::GetSetterNodeMenuName(SourceVariable);
}

void UCadenceUserVariableSetterNode::SetSourceVariable(UCadenceVariable* InVariable)
{
	if(SourceVariable != InVariable)
	{
		Super::SetSourceVariable(InVariable);
		
		if(UCadenceGraphNodePin* Pin = GetVariableInputPin())
		{
			Pin->ClearConnections();
			Pin->SetVariableClass(InVariable->GetClass());
			if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(InVariable))
			{
				Pin->GetVariable<UCadenceVariableArray>()->SetVariableClass(ArrayVariable->GetVariableClass());
			}
		}
		
		if(UCadenceGraphNodePin* Pin = GetVariableOutputPin())
		{
			Pin->ClearConnections();
			Pin->SetVariableClass(InVariable->GetClass());
			if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(InVariable))
			{
				Pin->GetVariable<UCadenceVariableArray>()->SetVariableClass(ArrayVariable->GetVariableClass());
			}
		}
	}
}
