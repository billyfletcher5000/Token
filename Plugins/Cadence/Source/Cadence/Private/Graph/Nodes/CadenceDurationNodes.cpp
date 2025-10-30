// Copyright (C) Billy Fletcher 2025


#include "Graph/Nodes/CadenceDurationNodes.h"

#include "CadenceAssetInstance.h"
#include "CadenceSubsystem.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceQuantizedDurationNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Period, UCadenceVariableQuantizationPeriod::StaticClass());
}

void UCadenceQuantizedDurationNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceQuantizedDurationNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Period);
	UCadenceVariableQuantizationPeriod* InVariable = InputPin->GetVariable<UCadenceVariableQuantizationPeriod>();
	EQuartzCommandQuantization Period = InVariable->GetValue<EQuartzCommandQuantization>();
	
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Duration);
	ensure(OutputPin);

	UCadenceVariableFloat* Variable = OutputPin->GetVariable<UCadenceVariableFloat>();
	Variable->SetValue(static_cast<float>(Period)); // TODO: Actually get the duration from the CadenceSubsystem
	
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
