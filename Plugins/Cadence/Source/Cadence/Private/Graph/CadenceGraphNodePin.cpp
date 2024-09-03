// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraphNodePin.h"

#include "Graph/CadenceVariable.h"

void UCadenceGraphNodePin::ConnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && !ConnectedPins.Contains(InPin))
	{
		Modify();
		ConnectedPins.Add(InPin);
	}
}

void UCadenceGraphNodePin::DisconnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && ConnectedPins.Contains(InPin))
	{
		Modify();
		ConnectedPins.Remove(InPin);
	}
}

void UCadenceGraphNodePin::ClearConnections()
{
	Modify();
	
	for(UCadenceGraphNodePin* OtherPin : ConnectedPins)
		OtherPin->DisconnectPin(this);
	
	ConnectedPins.Empty();
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

