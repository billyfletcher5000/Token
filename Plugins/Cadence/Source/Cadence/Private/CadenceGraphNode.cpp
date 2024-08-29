// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphNode.h"

#include "CadenceGraphNodePin.h"
#include "CadencePinConstants.h"


void UCadenceGraphNode::CreateInputPins()
{
	if(!IsPure() && HasInputExecPin())
	{
		AddInputExecPin(FCadencePinConstants::Pin_Default_Exec);
	}
}

void UCadenceGraphNode::CreateOutputPins()
{
	if(!IsPure() && HasOutputExecPin())
	{
		AddOutputExecPin(FCadencePinConstants::Pin_Default_Then);
	}
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetExecPin() const
{
	return GetInputPin(FCadencePinConstants::Pin_Default_Exec);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetThenPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Default_Then);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetInputPin(const FName& InPinName) const
{
	return GetPinFromArray(InputPins, InPinName);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetOutputPin(const FName& InPinName) const
{
	return GetPinFromArray(OutputPins, InPinName);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputExecPin(const FName& InPinName)
{
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateExecPin(InPinName);
	InputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputExecPin(const FName& InPinName)
{
	ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateExecPin(InPinName);
	OutputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{	
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	InputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{
	ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	OutputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateExecPin(const FName& InPinName)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(true);
	Pin->GenerateGUID();

	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateVariablePin(const FName& InPinName,const TObjectPtr<UClass>& InVariableClass)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(false);
	Pin->SetVariableClass(InVariableClass);
	Pin->GenerateGUID();

	return Pin;
}

bool UCadenceGraphNode::RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(InputPins.Contains(InPin))
	{
		InputPins.Remove(InPin);
		return true;
	}

	return false;
}

bool UCadenceGraphNode::RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(OutputPins.Contains(InPin))
	{
		OutputPins.Remove(InPin);
		return true;
	}

	return false;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetPinFromArray(const TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName)
{
	auto Result = InPinArray.FindByPredicate([&InPinName](const TObjectPtr<UCadenceGraphNodePin>& Pin)
	{
		return Pin->GetPinName() == InPinName;
	});

	if(Result != nullptr)
		return *Result;

	return nullptr;
}
