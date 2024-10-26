// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceTriggerNodes.h"

#include "CadenceSubsystem.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"
#include "Triggers/CadenceTrigger.h"

void UCadenceWaitForTriggerNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

void UCadenceWaitForTriggerNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputPin = GetInputPin(FCadencePinConstants::Pin_Trigger);
	UCadenceVariableTrigger* TriggerVariable = InputPin->GetVariable<UCadenceVariableTrigger>();

	UCadenceTriggerData* TriggerData = TriggerVariable->GetValue();
	InActionList.Add(TriggerData->CreateRunner());
}

void UCadenceTriggerSequenceNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddUserInputPin();
	AddUserInputPin();
}

void UCadenceTriggerSequenceNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

ECadenceNodeExecuteResult UCadenceTriggerSequenceNode::Execute(UCadenceContext* InContext)
{
	UCadenceTriggerSequenceData* SequenceData = NewObject<UCadenceTriggerSequenceData>(this);

	for(UCadenceGraphNodePin* InputPin : InputPins)
	{
		if(InputPin->IsExec())
			continue;

		UCadenceVariableTrigger* InputVariable = InputPin->GetVariable<UCadenceVariableTrigger>();
		if(InputVariable == nullptr)
			return ECadenceNodeExecuteResult::Failed;
		
		SequenceData->TriggerList.Add(InputVariable->GetValue());
	}

	if(SequenceData->TriggerList.Num() == 0)
	{
		UE_LOG(LogCadence, Warning, TEXT("Sequence trigger does not have any triggers added, so it will not do anything!"));
		return ECadenceNodeExecuteResult::Failed;
	}

	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Trigger);
	UCadenceVariableTrigger* OutputVariable = OutputPin->GetVariable<UCadenceVariableTrigger>();
	OutputVariable->SetValue(SequenceData);
	
	return ECadenceNodeExecuteResult::Complete;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceTriggerSequenceNode::AddUserInputPin()
{
	FString PinName = FCadencePinConstants::Pin_Trigger.ToString() + " " + GetNameForAdditionalPin(PinIndex).ToString();
	PinIndex++;	
	return AddInputVariablePin(FName(PinName), UCadenceVariableTrigger::StaticClass());
}

bool UCadenceTriggerSequenceNode::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{
	return RemoveInputPin(Pin);
}

bool UCadenceTriggerSequenceNode::CanRemovePin(const UCadenceGraphNodePin* Pin) const
{
	return !Pin->IsExec() && InputPins.Contains(Pin) && InputPins.Num() > 2;
}

void UCadenceQuantizedTimeTriggerNode::CreateOutputPins()
{	
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

ECadenceNodeExecuteResult UCadenceQuantizedTimeTriggerNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Trigger);
	UCadenceVariableTrigger* OutputVariable = OutputPin->GetVariable<UCadenceVariableTrigger>();

	UCadenceTriggerQuantizedTimeData* Data = NewObject<UCadenceTriggerQuantizedTimeData>();
	Data->TimePeriod = TimePeriod;
	Data->Count = Count;
	
	OutputVariable->SetValue(Data);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceSequenceSectionTriggerNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Trigger, UCadenceVariableTrigger::StaticClass());
}

ECadenceNodeExecuteResult UCadenceSequenceSectionTriggerNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Trigger);
	UCadenceVariableTrigger* OutputVariable = OutputPin->GetVariable<UCadenceVariableTrigger>();

	UCadenceTriggerSequenceSectionData* Data = NewObject<UCadenceTriggerSequenceSectionData>();
	Data->AssetInstance = InContext->AssetInstance;
	Data->SectionName = Section.Name;
	Data->Count = Count;
	
	OutputVariable->SetValue(Data);
	
	return ECadenceNodeExecuteResult::Complete;
}
