// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraphNodePin.h"

#include "Graph/CadenceVariable.h"

void UCadenceGraphNodePin::ConnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && !ConnectedPins.Contains(InPin))
	{
		Modify();
		ConnectedPins.Add(InPin);

		OnPinConnected.Broadcast(InPin);
	}
}

void UCadenceGraphNodePin::DisconnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && ConnectedPins.Contains(InPin))
	{
		Modify();
		ConnectedPins.Remove(InPin);

		OnPinDisconnected.Broadcast(InPin);

		if(ConnectedPins.Num() == 0)
			OnConnectionsCleared.Broadcast();
	}
}

void UCadenceGraphNodePin::ClearConnections()
{
	if(ConnectedPins.Num() == 0)
		return;
	
	Modify();
	
	for(UCadenceGraphNodePin* OtherPin : ConnectedPins)
		OtherPin->DisconnectPin(this);
	
	ConnectedPins.Empty();

	OnConnectionsCleared.Broadcast();
}

void UCadenceGraphNodePin::PruneConnections()
{
	for(UCadenceGraphNodePin* Pin : ConnectedPins)
	{
		if(Pin != nullptr && Pin->GetParentNode() == nullptr)
		{						
			Pin->Rename(nullptr, GetTransientPackage(), REN_ForceNoResetLoaders | REN_DoNotDirty | REN_DontCreateRedirectors | REN_NonTransactional);
			Pin->MarkAsGarbage();
		}
	}
	
	ConnectedPins.RemoveAll([](const TObjectPtr<UCadenceGraphNodePin>& Pin)
	{
		return Pin == nullptr || Pin->GetParentNode() == nullptr;
	});
}

void UCadenceGraphNodePin::GenerateGUID()
{
	GUID = FGuid::NewGuid();
}

void UCadenceGraphNodePin::SetVariableClass(const TSubclassOf<UCadenceVariable>& InVariableClass)
{
	if(VariableClass != InVariableClass)
	{
		Modify();
		VariableClass = InVariableClass;
		Variable = nullptr;
	}
}

void UCadenceGraphNodePin::SetVariableSecondaryClass(const TSubclassOf<UCadenceVariable>& InVariableClass)
{
	if(VariableSecondaryClass != InVariableClass)
	{
		Modify();
		VariableSecondaryClass = InVariableClass;
		Variable = nullptr;
	}
}

void UCadenceGraphNodePin::SetVariable(UCadenceVariable* InVariable)
{
	if(InVariable->IsA(VariableClass))
		Variable = InVariable;
}

