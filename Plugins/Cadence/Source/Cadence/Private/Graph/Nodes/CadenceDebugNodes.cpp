// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceDebugNodes.h"

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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, *Output);
	
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, *Output);
	
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, *Output);
	
	return ECadenceNodeExecuteResult::Complete;
}
