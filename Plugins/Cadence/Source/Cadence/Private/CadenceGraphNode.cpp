// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphNode.h"

#include "CadenceGraphNodePin.h"


TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetInputPin(const FName& InPinName)
{
	return GetPin(InputPins, InPinName);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetOutputPin(const FName& InPinName)
{
	return GetPin(OutputPins, InPinName);
}

void UCadenceGraphNode::AddInputPin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{
	
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	InputPins.Add(CreatePin(InPinName, InVariableClass));
}

void UCadenceGraphNode::AddOutputPin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	OutputPins.Add(CreatePin(InPinName, InVariableClass));
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreatePin(const FName& InPinName,const TObjectPtr<UClass>& InVariableClass)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>();
	
	Pin->SetPinName(InPinName);
	Pin->SetVariableClass(InVariableClass);
	Pin->GenerateGUID();

	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetPin(TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName)
{
	auto Result = InPinArray.FindByPredicate([&InPinName](const TObjectPtr<UCadenceGraphNodePin>& Pin)
	{
		return Pin->GetPinName() == InPinName;
	});

	if(Result != nullptr)
		return *Result;

	return nullptr;
}
