// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceRerouteNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"
/*
ECadenceNodeExecuteResult UCadenceRerouteNodeBase::Execute(UCadenceContext* InContext)
{
	if(!bIsExecReroute && VariableType != nullptr)
	{
		UCadenceGraphNodePin* InputPin = GetRerouteInputPin();
		UCadenceGraphNodePin* OutputPin = GetRerouteOutputPin();
		
		UE_LOG(LogCadence, Log, TEXT("UCadenceSimpleRerouteNode::Execute Pre-CopyValue: %s - %s"), *GetDebugName(), *InputPin->GetGUID().ToString());
		InContext->ParentNode = this;
		OutputPin->GetVariable()->CopyValueFrom(InputPin->GetVariable(), InContext);
	}
	
	return ECadenceNodeExecuteResult::Complete;
}*/

void UCadenceRerouteNodeBase::SetAsExecReroute()
{
	if(!bIsExecReroute)
	{
		bIsExecReroute = true;
		RemoveAllPins();
		AddInputExecPin(FCadencePinConstants::Pin_Default_Exec);
		AddOutputExecPin(FCadencePinConstants::Pin_Default_Then);
	}
}

void UCadenceRerouteNodeBase::SetVariableType(const TSubclassOf<UCadenceVariable>& InVariableType)
{
	if(VariableType != InVariableType)
	{
		VariableType = InVariableType;
		RemoveAllPins();
		AddInputVariablePin(FCadencePinConstants::Pin_Wildcard, VariableType);
		AddOutputVariablePin(FCadencePinConstants::Pin_Wildcard, VariableType);
	}
}

void UCadenceRerouteNodeBase::Clear()
{
	if(VariableType != nullptr)
	{
		VariableType = nullptr;
	}

	bIsExecReroute = false;	
	RemoveAllPins();
}

void UCadenceRerouteNodeBase::CheckRerouteTypeValid()
{
	if(bIsExecReroute || VariableType != nullptr)
	{
		if(!GetRerouteInputPin()->HasConnections() && !GetRerouteOutputPin()->HasConnections())
			Clear();
	}
}

UCadenceGraphNodePin* UCadenceRerouteNodeBase::GetRerouteInputPin() const
{	
	return GetInputPin(FCadencePinConstants::Pin_Wildcard);
}

UCadenceGraphNodePin* UCadenceRerouteNodeBase::GetRerouteOutputPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Wildcard);
}

UCadenceGraphNode* UCadenceRerouteNodeBase::GetRerouteInputNode() const
{
	if(const UCadenceGraphNodePin* Pin = GetRerouteInputPin())
	{
		auto& ConnectedPins = Pin->GetConnectedPins();
		if(ConnectedPins.Num() > 0)
		{
			UCadenceGraphNode* ConnectedNode = ConnectedPins[0]->GetParentNode();
			if(ConnectedNode->IsReroute())
			{
				const UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(ConnectedNode);
				ensure(RerouteNode);
				return RerouteNode->GetRerouteInputNode();
			}

			return ConnectedNode;
		}
	}
	
	return nullptr;
}

TArray<UCadenceGraphNodePin*> UCadenceRerouteNodeBase::GetRerouteOutputNodeConnectedInputPins() const
{
	TArray<UCadenceGraphNodePin*> Result;

	GetRerouteOutputNodeConnectedInputPins(Result);

	return Result;
}

void UCadenceRerouteNodeBase::GetRerouteOutputNodeConnectedInputPins(TArray<UCadenceGraphNodePin*>& InResult) const
{
	UCadenceGraphNodePin* OutputPin = GetRerouteOutputPin();

	auto& ConnectedInputPins = OutputPin->GetConnectedPins();
	for (UCadenceGraphNodePin* ConnectedInputPin : ConnectedInputPins)
	{
		UCadenceGraphNode* ConnectedNode = ConnectedInputPin->GetParentNode();
		if(ConnectedNode->IsReroute())
		{
			const UCadenceRerouteNodeBase* RerouteNode = Cast<UCadenceRerouteNodeBase>(ConnectedNode);
			ensure(RerouteNode);
			RerouteNode->GetRerouteOutputNodeConnectedInputPins(InResult);
		}
		else
		{
			InResult.Add(ConnectedInputPin);
		}
	}
}
