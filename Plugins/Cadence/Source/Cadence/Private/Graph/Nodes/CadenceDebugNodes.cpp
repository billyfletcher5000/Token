// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceDebugNodes.h"

#include "Cadence.h"
#include "CadenceContext.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadenceGraphRunner.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

void UCadenceDebugIntNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceDebugIntNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Int);
	ensure(Pin);

	UCadenceVariableInt* Variable = Pin->GetVariable<UCadenceVariableInt>();
	ensure(Variable);

	const FString Output = Variable->ConvertToValueString();
	
	ensure(GEngine);
	UE_LOG(LogCadence, Log, TEXT("Debug Node Output: %s"), *Output);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, Variable->GetPinColor().ToFColor(false), *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceDebugFloatNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Float, UCadenceVariableFloat::StaticClass());	
}

ECadenceNodeExecuteResult UCadenceDebugFloatNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Float);
	ensure(Pin);

	UCadenceVariableFloat* Variable = Pin->GetVariable<UCadenceVariableFloat>();
	ensure(Variable);

	FString Output = FString::SanitizeFloat(Variable->GetValue());
	
	ensure(GEngine);
	UE_LOG(LogCadence, Log, TEXT("Debug Node Output: %s"), *Output);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, Variable->GetPinColor().ToFColor(false), *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceDebugStringNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_String, UCadenceVariableString::StaticClass());	
}

ECadenceNodeExecuteResult UCadenceDebugStringNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_String);
	ensure(Pin);

	UCadenceVariableString* Variable = Pin->GetVariable<UCadenceVariableString>();
	ensure(Variable);

	FString Output = Variable->GetValue();
	
	ensure(GEngine);
	UE_LOG(LogCadence, Log, TEXT("Debug Node Output: %s"), *Output);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, Variable->GetPinColor().ToFColor(false), *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceDebugActorNameNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());	
}

ECadenceNodeExecuteResult UCadenceDebugActorNameNode::Execute(UCadenceContext* InContext)
{	
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* Variable = Pin->GetVariable<UCadenceVariableActor>();

	FString Output = Variable->GetValue()->GetName();
	
	ensure(GEngine);
	UE_LOG(LogCadence, Log, TEXT("Debug Node Output: %s"), *Output);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, Variable->GetPinColor().ToFColor(false), *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceDebugQuantizationPeriodNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Enum, UCadenceVariableEnum::StaticClass());	
}

ECadenceNodeExecuteResult UCadenceDebugQuantizationPeriodNode::Execute(UCadenceContext* InContext)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = GetInputPin(FCadencePinConstants::Pin_Enum);
	UCadenceVariableEnum* Variable = Pin->GetVariable<UCadenceVariableEnum>();

	FString Output;
	if(!Pin->HasConnections())
	{
		Output = "No enum connected!";
	}
	else
	{
		UCadenceVariableEnum* ConnectedVariable = Pin->GetConnectedPins()[0]->GetVariable<UCadenceVariableEnum>();		
		Output = ConnectedVariable->ConvertToValueString();
	}	
	
	ensure(GEngine);
	UE_LOG(LogCadence, Log, TEXT("Debug Node Output: %s"), *Output);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, Variable->GetPinColor().ToFColor(false), *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceTestIncrementIntNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableInt>(FCadencePinConstants::Pin_Value, 1);
}

void UCadenceTestIncrementIntNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Int, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceTestIncrementIntNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InIntPin = GetInputPin(FCadencePinConstants::Pin_Int);
	int32 InInt = InIntPin->GetVariable<UCadenceVariableInt>()->GetValue();

	UCadenceGraphNodePin* InValuePin = GetInputPin(FCadencePinConstants::Pin_Value);
	int32 InValue = InValuePin->GetVariable<UCadenceVariableInt>()->GetValue();

	UCadenceGraphNodePin* OutIntPin = GetOutputPin(FCadencePinConstants::Pin_Int);
	UCadenceVariableInt* OutVariable = OutIntPin->GetVariable<UCadenceVariableInt>();

	OutVariable->SetValue(InInt + InValue);
	
	return ECadenceNodeExecuteResult::Complete;
}
