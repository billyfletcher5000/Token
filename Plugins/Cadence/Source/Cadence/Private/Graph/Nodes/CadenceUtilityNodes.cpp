// Copyright (C) Billy Fletcher 2025


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
