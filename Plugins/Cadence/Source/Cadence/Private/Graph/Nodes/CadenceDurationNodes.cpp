// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceDurationNodes.h"

#include "CadenceSubsystem.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceQuantizedDurationNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceQuantizedDurationNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(FCadencePinConstants::Pin_Duration);
	ensure(Pin);

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
	Variable->SetValue(0.69f);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceSequenceSectionDurationNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceSequenceSectionDurationNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Duration);
	UCadenceVariableFloat* FloatVariable = OutputPin->GetVariable<UCadenceVariableFloat>();
		
	float Duration = InContext->AssetInstance->GetSectionDuration(Section.Name);
	FloatVariable->SetValue(Duration);
	
	return ECadenceNodeExecuteResult::Complete;
}
