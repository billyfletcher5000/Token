// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceUtilityNodes.h"

#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceKeepAliveActorNode::CreateInputPins()
{
	Super::CreateInputPins();

	AddInputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
}

ECadenceNodeExecuteResult UCadenceKeepAliveActorNode::Execute(UCadenceContext* InContext)
{
	return ECadenceNodeExecuteResult::Complete;
}
