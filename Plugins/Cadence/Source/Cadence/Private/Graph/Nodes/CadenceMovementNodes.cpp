// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceMovementNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"
#include "TickableActions/CadenceMoveTickableActions.h"

void UCadenceRotateToNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_Rotation, UCadenceVariableRotator::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

void UCadenceRotateToNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList, UCadenceContext* InContext)
{
	UCadenceGraphNodePin* ActorPin = GetInputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* ActorVariable = ActorPin->GetVariable<UCadenceVariableActor>();
	AActor* Actor = ActorVariable->GetValue();
	
	UCadenceGraphNodePin* RotationPin = GetInputPin(FCadencePinConstants::Pin_Rotation);
	UCadenceGraphNodePin* DurationPin = GetInputPin(FCadencePinConstants::Pin_Duration);

	FRotator TargetRotation = RotationPin->GetVariable<UCadenceVariableRotator>()->GetValue();	

	TargetRotation = TransformRotation(TargetRotation, Actor);

	float TargetDuration = DurationPin->GetVariable<UCadenceVariableFloat>()->GetValue();	

	InActionList.Add(UCadenceActorRotateTickable::Create(Actor, TargetDuration, TargetRotation, Easing, bUseShortestPath));
}
