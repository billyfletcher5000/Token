// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceStageNodes.h"

#include "CadenceSubsystem.h"
#include "Transitions/CadenceTransitionTransform.h"

void UCadenceStageTransitionNode::CreateInputPins()
{
	Super::CreateInputPins();

	AddInputVariablePin(FCadenceStageNodeConstants::Pin_Snapshot, UCadenceVariablePlacesSnapshot::StaticClass());
	AddInputVariablePin(FCadenceStageNodeConstants::Pin_Transition, UCadenceVariableTransitionTransform::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadencePinConstants::Pin_Duration, 1.0f);
}

void UCadenceStageTransitionNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList,
	UCadenceContext* InContext)
{
	UCadencePlacesSnapshot* Snapshot = nullptr;
	if(!GetInputPinValue<UCadenceVariablePlacesSnapshot, UCadencePlacesSnapshot*>(FCadenceStageNodeConstants::Pin_Snapshot, Snapshot))
		return;

	UCadenceTransitionTransform* Transition = nullptr;
	if(!GetInputPinValue<UCadenceVariableTransitionTransform, UCadenceTransitionTransform*>(FCadenceStageNodeConstants::Pin_Transition, Transition))
		return;

	float Duration = 1.0f;
	if(!GetInputPinValue<UCadenceVariableFloat, float>(FCadencePinConstants::Pin_Duration, Duration))
		return;

	UWorld* World = GetWorld();
	if(!IsValid(World))
		return;

	UCadenceSubsystem* CadenceSubsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!IsValid(CadenceSubsystem))
		return;

	for(FCadencePlacePair& PlacePair : Snapshot->Places)
	{
		AActor* Actor = CadenceSubsystem->GetTrackedActor(PlacePair.TrackedActorID);
		if(!IsValid(Actor))
			continue;

		InActionList.Append(Transition->CreateTickables(Actor, Duration, PlacePair.Place));
	}	
}
