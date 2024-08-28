// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphNodePin.h"

void UCadenceGraphNodePin::ConnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && !ConnectedPins.Contains(InPin))
	{
		ConnectedPins.Add(InPin);
	}
}

void UCadenceGraphNodePin::DisconnectPin(UCadenceGraphNodePin* InPin)
{
	if(ensure(InPin) && ConnectedPins.Contains(InPin))
	{
		ConnectedPins.Remove(InPin);
	}
}

void UCadenceGraphNodePin::ClearConnections()
{
	ConnectedPins.Empty();
}

void UCadenceGraphNodePin::GenerateGUID()
{
	GUID = FGuid::NewGuid();
}
