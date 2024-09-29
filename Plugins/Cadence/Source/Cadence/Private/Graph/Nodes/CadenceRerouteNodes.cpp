// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceRerouteNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

ECadenceNodeExecuteResult UCadenceSimpleRerouteNode::Execute(UCadenceContext* InContext)
{
	if(!bIsExecReroute && VariableType != nullptr)
	{
		UCadenceGraphNodePin* InputPin = GetRerouteInputPin();
		UCadenceGraphNodePin* OutputPin = GetRerouteOutputPin();

		OutputPin->GetVariable()->CopyValueFrom(InputPin->GetVariable());
	}	
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceSimpleRerouteNode::SetAsExecReroute()
{
	if(!bIsExecReroute)
	{
		bIsExecReroute = true;
		RemoveAllPins();
	}
}

void UCadenceSimpleRerouteNode::SetVariableType(const TSubclassOf<UCadenceVariable>& InVariableType)
{
	if(VariableType != InVariableType)
	{
		VariableType = InVariableType;
		RemoveAllPins();
		AddInputVariablePin(FCadencePinConstants::Pin_Wildcard, VariableType);
		AddOutputVariablePin(FCadencePinConstants::Pin_Wildcard, VariableType);
	}
}

void UCadenceSimpleRerouteNode::Clear()
{
	if(VariableType != nullptr)
	{
		VariableType = nullptr;
	}

	bIsExecReroute = false;	
	RemoveAllPins();
}

void UCadenceSimpleRerouteNode::CheckRerouteTypeValid()
{
	if(bIsExecReroute || VariableType != nullptr)
	{
		if(!GetRerouteInputPin()->HasConnections() && !GetRerouteOutputPin()->HasConnections())
			Clear();
	}
}

UCadenceGraphNodePin* UCadenceSimpleRerouteNode::GetRerouteInputPin() const
{	
	return GetInputPin(FCadencePinConstants::Pin_Wildcard);
}

UCadenceGraphNodePin* UCadenceSimpleRerouteNode::GetRerouteOutputPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Wildcard);
}
