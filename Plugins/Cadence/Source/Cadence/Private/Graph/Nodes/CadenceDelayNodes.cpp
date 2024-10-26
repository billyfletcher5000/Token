// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceDelayNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"
#include "TickableActions/CadenceTimeTickableActions.h"

void UCadenceDelayNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

void UCadenceDelayNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Duration);
	UCadenceVariableFloat* InputFloatVariable = InputPin->GetVariable<UCadenceVariableFloat>();

	float Duration = InputFloatVariable->GetValue();
	InActionList.Add(UCadenceDelayTickable::Create(Duration));
}