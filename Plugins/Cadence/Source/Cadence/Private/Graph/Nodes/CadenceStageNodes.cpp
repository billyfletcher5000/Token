// Copyright (C) Billy Fletcher 2025


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

	for(FCadenceActorPlaceSet& PlaceSet : Snapshot->PlaceSets)
	{
		InActionList.Append(Transition->CreateTickables(World, PlaceSet, Duration));
	}	
}
