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

void UCadenceGraphNode::AddInputExecPin(const FName& InPinName)
{
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	InputPins.Add(CreateExecPin(InPinName));
}

void UCadenceGraphNode::AddOutputExecPin(const FName& InPinName)
{
	ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	OutputPins.Add(CreateExecPin(InPinName));
}

void UCadenceGraphNode::AddInputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{	
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	InputPins.Add(CreateVariablePin(InPinName, InVariableClass));
}

void UCadenceGraphNode::AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{
	ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	OutputPins.Add(CreateVariablePin(InPinName, InVariableClass));
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

void UCadenceGraphNode::RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(InputPins.Contains(InPin))
		InputPins.Remove(InPin);
}

void UCadenceGraphNode::RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(OutputPins.Contains(InPin))
		OutputPins.Remove(InPin);
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
