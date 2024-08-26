// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphNode.h"

void UCadenceGraphNodePin::SetConnectedPins(TArray<TObjectPtr<UCadenceGraphNodePin>> InPins)
{
	ConnectedPins = InPins;
}

void UCadenceGraphNodePin::ConnectPin(UCadenceGraphNodePin* InPin)
{
	ensure(InPin);	
	ensure(!ConnectedPins.Contains(InPin));

	ConnectedPins.Add(InPin);
}

void UCadenceGraphNodePin::DisconnectPin(UCadenceGraphNodePin* InPin)
{	
	ensure(InPin);	
	ensure(ConnectedPins.Contains(InPin));

	ConnectedPins.Remove(InPin);
}