// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraphNode.h"

#include "Algo/ForEach.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"


void UCadenceGraphNode::GenerateGUID()
{
	GUID = FGuid::NewGuid();
	DebugName = GetName();
}

void UCadenceGraphNode::DestroyNode()
{
	for(UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	for(UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();

	InputPins.Empty();
	OutputPins.Empty();
	
	ParentGraph->RemoveNode(this);
}

void UCadenceGraphNode::CreateInputPins()
{
	if(!IsPure() && HasInputExecPin())
	{
		AddInputExecPin(FCadencePinConstants::Pin_Default_Exec);
	}
}

void UCadenceGraphNode::CreateOutputPins()
{
	if(!IsPure() && ShouldCreateThenExecPin())
	{
		AddOutputExecPin(FCadencePinConstants::Pin_Default_Then);
	}
}

void UCadenceGraphNode::ClearConnections()
{
	for(UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	for(UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();
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

UCadenceGraphNodePin* UCadenceGraphNode::GetMostAppropriateAutomaticInputPin(UCadenceGraphNodePin* OtherPin)
{
	for(UCadenceGraphNodePin* MyPin : InputPins)
	{
		if(MyPin->IsExec() && OtherPin->IsExec())
			return MyPin;

		if(MyPin->GetVariableClass() == OtherPin->GetVariableClass())
			return MyPin;
	}

	return nullptr;
}

UCadenceGraphNodePin* UCadenceGraphNode::GetMostAppropriateAutomaticOutputPin(UCadenceGraphNodePin* OtherPin)
{
	for(UCadenceGraphNodePin* MyPin : OutputPins)
	{
		if(MyPin->IsExec() && OtherPin->IsExec())
			return MyPin;

		if(MyPin->GetVariableClass() == OtherPin->GetVariableClass())
			return MyPin;
	}

	return nullptr;
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
	if(!ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin")))
		return nullptr;
		
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	InputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const bool& InIsMaster)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, InIsMaster);
	InputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId, const bool& InIsMaster)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, InIsMaster, true);
	InputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const bool& bInOptional)
{
	if(!ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin")))
		return nullptr;
		
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	OutputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, false);
	OutputPins.Add(Pin);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, false, true);
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

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(false);
	Pin->SetVariableClass(InVariableClass);
	Pin->GenerateGUID();

	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const bool& InIsMaster, const bool InIsArray)
{
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(false);
	Pin->SetVariableClass(InIsArray ? UCadenceVariableArray::StaticClass() : nullptr);
	Pin->SetWildcardId(InWildcardId);
	Pin->GenerateGUID();

	if(InIsMaster)
	{
		ensureMsgf(InWildcardId >= 0, TEXT("Wildcard Id must be a positive number, negative numbers are reserved for initialisation checks"));
		ensureMsgf(!WildcardIdToMasterPin.Contains(InWildcardId), TEXT("Master pin already exists for wildcard ID: %d"), InWildcardId);
		WildcardIdToMasterPin.Add(InWildcardId, Pin);

		Pin->OnPinConnected.AddUObject(this, &UCadenceGraphNode::OnPinConnectedToWildcardMaster, Pin);
		Pin->OnConnectionsCleared.AddUObject(this, &UCadenceGraphNode::OnPinConnectionsClearedFromWildcardMaster, Pin);
	}
	
	return Pin;
}

bool UCadenceGraphNode::RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(InputPins.Contains(InPin))
	{
		InPin->ClearConnections();
		InputPins.Remove(InPin);
		return true;
	}

	return false;
}

bool UCadenceGraphNode::RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(OutputPins.Contains(InPin))
	{
		InPin->ClearConnections();
		OutputPins.Remove(InPin);
		return true;
	}

	return false;
}

void UCadenceGraphNode::RemoveAllInputPins()
{
	for (UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	InputPins.Empty();
}

void UCadenceGraphNode::RemoveAllOutputPins()
{
	for (UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();

	OutputPins.Empty();
}

void UCadenceGraphNode::RemoveAllPins()
{
	RemoveAllInputPins();
	RemoveAllOutputPins();
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

void UCadenceGraphNode::OnPinConnectedToWildcardMaster(UCadenceGraphNodePin* InConnectedPin, UCadenceGraphNodePin* InMasterPin)
{
	if(!ensure(InConnectedPin))
		return;

	if(!ensure(InMasterPin))
		return;

	UClass* TargetClass = nullptr;
	if(InConnectedPin->GetVariableClass() == UCadenceVariableArray::StaticClass())
	{
		UCadenceVariable* ConnectedVar = InConnectedPin->GetVariable(false);
		if(ensure(ConnectedVar))
		{
			UCadenceVariableArray* ConnectedArray = Cast<UCadenceVariableArray>(ConnectedVar);
			TargetClass = ConnectedArray->GetVariableClass();
		}
	}
	else
	{
		TargetClass = InConnectedPin->GetVariableClass();
	}

	if(!ensure(TargetClass))
		return;

	int32 MasterWildcardId = InMasterPin->GetWildcardId();

	auto SetClassLambda = [&MasterWildcardId, &TargetClass] (UCadenceGraphNodePin* Pin)
	{
		if(Pin->GetWildcardId() == MasterWildcardId)
		{
			if(Pin->GetVariableClass() == UCadenceVariableArray::StaticClass())
			{
				UCadenceVariableArray* PinArray = Pin->GetVariable<UCadenceVariableArray>();
				PinArray->SetVariableClass(TargetClass);
			}
			else
			{
				Pin->SetVariableClass(TargetClass);
			}
		}
	};
	
	Algo::ForEach(InputPins, SetClassLambda);
	Algo::ForEach(OutputPins, SetClassLambda);

#if WITH_EDITOR
	GetParentGraph()->NotifyPinTypesChanged();
#endif
}

void UCadenceGraphNode::OnPinConnectionsClearedFromWildcardMaster(UCadenceGraphNodePin* InMasterPin)
{
	int32 MasterWildcardId = InMasterPin->GetWildcardId();

	auto SetClassLambda = [&MasterWildcardId] (UCadenceGraphNodePin* Pin)
	{
		if(Pin->GetWildcardId() == MasterWildcardId)
		{
			if(Pin->GetVariableClass() == UCadenceVariableArray::StaticClass())
			{
				UCadenceVariableArray* PinArray = Pin->GetVariable<UCadenceVariableArray>();
				PinArray->SetVariableClass(nullptr);
			}
			else
			{
				Pin->SetVariableClass(nullptr);
			}
		}
	};
	
	Algo::ForEach(InputPins, SetClassLambda);
	Algo::ForEach(OutputPins, SetClassLambda);

#if WITH_EDITOR
	GetParentGraph()->NotifyPinTypesChanged();
#endif
}
