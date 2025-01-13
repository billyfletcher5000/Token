// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/AudioSynesthesia/CadenceSynesthesiaNodes.h"

#include "Graph/AudioSynesthesia/CadenceSynesthesiaVariables.h"

void UCadenceGetLoudnessAtTimeNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadenceSynesthesiaConstants::Pin_LoudnessNRT, UCadenceVariableLoudnessNRT::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadenceSynesthesiaConstants::Pin_Seconds, 0.0f);
	AddInputVariablePinDefault<UCadenceVariableInt>(FCadenceSynesthesiaConstants::Pin_Channel, INDEX_NONE);
	AddInputVariablePinDefault<UCadenceVariableBool>(FCadenceSynesthesiaConstants::Pin_Normalized, true);
}

void UCadenceGetLoudnessAtTimeNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadenceSynesthesiaConstants::Pin_Loudness, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGetLoudnessAtTimeNode::Execute(UCadenceContext* InContext)
{
	ULoudnessNRT* LoudnessNRT = nullptr;
	if(!GetInputPinValue<UCadenceVariableLoudnessNRT, ULoudnessNRT*>(FCadenceSynesthesiaConstants::Pin_LoudnessNRT, LoudnessNRT))
		return ECadenceNodeExecuteResult::Failed;

	float Seconds = 0.0f;
	int32 Channel = INDEX_NONE;
	bool IsNormalized = false;

	if(!GetInputPinValue<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_Seconds, Seconds))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableInt, int32>(FCadenceSynesthesiaConstants::Pin_Channel, Channel))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableBool, bool>(FCadenceSynesthesiaConstants::Pin_Normalized, IsNormalized))
		return ECadenceNodeExecuteResult::Failed;
		
	float OutLoudness = 0.0f;
	if(IsNormalized)
		LoudnessNRT->GetNormalizedChannelLoudnessAtTime(Seconds, Channel, OutLoudness);
	else
		LoudnessNRT->GetChannelLoudnessAtTime(Seconds, Channel, OutLoudness);

	if(!SetOutputPinValue<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_Loudness, OutLoudness))
		return ECadenceNodeExecuteResult::Failed;
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceGetConstantQAtTimeNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadenceSynesthesiaConstants::Pin_ConstantQNRT, UCadenceVariableConstantQNRT::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadenceSynesthesiaConstants::Pin_Seconds, 0.0f);
	AddInputVariablePinDefault<UCadenceVariableInt>(FCadenceSynesthesiaConstants::Pin_Channel, 0);
	AddInputVariablePinDefault<UCadenceVariableBool>(FCadenceSynesthesiaConstants::Pin_Normalized, true);
}

void UCadenceGetConstantQAtTimeNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePinArray(FCadenceSynesthesiaConstants::Pin_ConstantQ, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGetConstantQAtTimeNode::Execute(UCadenceContext* InContext)
{
	UConstantQNRT* ConstantQNRT = nullptr;
	if(!GetInputPinValue<UCadenceVariableConstantQNRT, UConstantQNRT*>(FCadenceSynesthesiaConstants::Pin_ConstantQNRT, ConstantQNRT))
		return ECadenceNodeExecuteResult::Failed;

	float Seconds = 0.0f;
	int32 Channel = 0;
	bool IsNormalized = false;

	if(!GetInputPinValue<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_Seconds, Seconds))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableInt, int32>(FCadenceSynesthesiaConstants::Pin_Channel, Channel))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableBool, bool>(FCadenceSynesthesiaConstants::Pin_Normalized, IsNormalized))
		return ECadenceNodeExecuteResult::Failed;
		
	TArray<float> OutConstantQ;
	if(IsNormalized)
		ConstantQNRT->GetNormalizedChannelConstantQAtTime(Seconds, Channel, OutConstantQ);
	else
		ConstantQNRT->GetChannelConstantQAtTime(Seconds, Channel, OutConstantQ);

	if(!SetOutputPinValueArray<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_ConstantQ, OutConstantQ))
		return ECadenceNodeExecuteResult::Failed;
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceGetOnsetsBetweenTimesNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadenceSynesthesiaConstants::Pin_OnsetNRT, UCadenceVariableOnsetNRT::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadenceSynesthesiaConstants::Pin_StartSeconds, 0.0f);
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadenceSynesthesiaConstants::Pin_EndSeconds, 0.0f);
	AddInputVariablePinDefault<UCadenceVariableInt>(FCadenceSynesthesiaConstants::Pin_Channel, 0);
	AddInputVariablePinDefault<UCadenceVariableBool>(FCadenceSynesthesiaConstants::Pin_Normalized, true);
}

void UCadenceGetOnsetsBetweenTimesNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePinArray(FCadenceSynesthesiaConstants::Pin_OnsetTimestamps, UCadenceVariableFloat::StaticClass());
	AddOutputVariablePinArray(FCadenceSynesthesiaConstants::Pin_OnsetStrengths, UCadenceVariableFloat::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGetOnsetsBetweenTimesNode::Execute(UCadenceContext* InContext)
{
	UOnsetNRT* OnsetNRT = nullptr;
	if(!GetInputPinValue<UCadenceVariableOnsetNRT, UOnsetNRT*>(FCadenceSynesthesiaConstants::Pin_OnsetNRT, OnsetNRT))
		return ECadenceNodeExecuteResult::Failed;

	float StartSeconds = 0.0f;
	float EndSeconds = 0.0f;
	int32 Channel = 0;
	bool IsNormalized = false;

	if(!GetInputPinValue<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_StartSeconds, StartSeconds))
		return ECadenceNodeExecuteResult::Failed;
	
	if(!GetInputPinValue<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_EndSeconds, EndSeconds))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableInt, int32>(FCadenceSynesthesiaConstants::Pin_Channel, Channel))
		return ECadenceNodeExecuteResult::Failed;

	if(!GetInputPinValue<UCadenceVariableBool, bool>(FCadenceSynesthesiaConstants::Pin_Normalized, IsNormalized))
		return ECadenceNodeExecuteResult::Failed;
		
	TArray<float> OutOnsetTimestamps;
	TArray<float> OutOnsetStrengths;
	
	if(IsNormalized)
		OnsetNRT->GetNormalizedChannelOnsetsBetweenTimes(StartSeconds, EndSeconds, Channel, OutOnsetTimestamps, OutOnsetStrengths);
	else
		OnsetNRT->GetChannelOnsetsBetweenTimes(StartSeconds, EndSeconds, Channel, OutOnsetTimestamps, OutOnsetStrengths);

	if(!SetOutputPinValueArray<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_OnsetTimestamps, OutOnsetTimestamps))
		return ECadenceNodeExecuteResult::Failed;
	
	if(!SetOutputPinValueArray<UCadenceVariableFloat, float>(FCadenceSynesthesiaConstants::Pin_OnsetStrengths, OutOnsetStrengths))
		return ECadenceNodeExecuteResult::Failed;
	
	return ECadenceNodeExecuteResult::Complete;
}
