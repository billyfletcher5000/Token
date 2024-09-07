// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceDelayNodes.h"

#include "CadenceContext.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceDelayNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceDelayNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Duration);
	UCadenceVariableFloat* InputFloatVariable = InputPin->GetVariable<UCadenceVariableFloat>();

	float Duration = InputFloatVariable->GetValue();
	float TimeElapsed = 0.0f;
	
	if(TimeElapsedMap.Contains(InContext->Pathway))
		TimeElapsed = TimeElapsedMap[InContext->Pathway];
	else
		TimeElapsedMap.Add(InContext->Pathway, 0.0f);

	TimeElapsed += InContext->DeltaSeconds;
	if(TimeElapsed > Duration)
	{
		TimeElapsedMap.Remove(InContext->Pathway);
		return ECadenceNodeExecuteResult::Complete;
	}
	else
	{
		TimeElapsedMap[InContext->Pathway] = TimeElapsed;
		return ECadenceNodeExecuteResult::Running;
	}
}
