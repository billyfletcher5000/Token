// Copyright (C) Billy Fletcher 2025


#include "Actors/CadenceActorLifetime.h"

#include "Cadence.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNode.h"

void UCadenceActorLifetimeManager::RegisterActor(AActor* InActor, const ECadenceActorLifetime& InLifetime, const FGuid& OutputPinVariableGUID)
{
	if(!ensure(InActor))
		return;

	FCadenceActorLifetimeData* ExistingData = Data.FindByPredicate([InActor](const FCadenceActorLifetimeData& Element) { return Element.Actor == InActor; });

	if(!ExistingData)
	{
		FCadenceActorLifetimeData LifetimeData;
		LifetimeData.Actor = InActor;
		LifetimeData.Lifetime = InLifetime;

		// We only ever add the initial creator node's pin once as it's the origin of the actor and not a result of an input that could be needed
		// multiple times if multiple pathways to the node exist
		// TODO: Sort out the documentation/readability of all this, it makes sense but I'm not explaining it great
		LifetimeData.UsageGUIDs.Add(OutputPinVariableGUID);
	
		UE_LOG(LogCadence, Log, TEXT("RegisterActor: Actor: %s GUID: %s"), *InActor->GetName(), *OutputPinVariableGUID.ToString());
	
		Data.Add(LifetimeData);
	}
	else
	{		
		// We only ever add the initial creator node's pin once as it's the origin of the actor and not a result of an input that could be needed
		// multiple times if multiple pathways to the node exist
		// TODO: Sort out the documentation/readability of all this, it makes sense but I'm not explaining it great
		ExistingData->UsageGUIDs.Add(OutputPinVariableGUID);
	}
}

void UCadenceActorLifetimeManager::RegisterActorUsage(AActor* InActor, const FGuid& PinVariableGUID, UCadenceGraphNode* InParentNode)
{
	FCadenceActorLifetimeData* LifetimeData = Data.FindByPredicate([InActor](const FCadenceActorLifetimeData& Element) { return Element.Actor == InActor; });

	if(LifetimeData && ensure(PinVariableGUID.IsValid()) && ensure(InParentNode))
	{
		int32 UsageCount = FMath::Max(InParentNode->GetParentGraph()->GetRootExecNodesThatLeadToNode(InParentNode).Num(), 1);		
		UE_LOG(LogCadence, Log, TEXT("RegisterActorUsage: Actor: %s Node: %s Count: %d GUID: %s"), *InActor->GetName(), *InParentNode->GetDebugName(), UsageCount, *PinVariableGUID.ToString());
		for(int32 UsageIndex = 0; UsageIndex < UsageCount; ++UsageIndex)
		{
			LifetimeData->UsageGUIDs.Add(PinVariableGUID);
		}
	}	
}

void UCadenceActorLifetimeManager::UnregisterActorUsage(AActor* InActor, const FGuid& UserGUID)
{
	FCadenceActorLifetimeData* LifetimeData = Data.FindByPredicate([InActor](const FCadenceActorLifetimeData& Element) { return Element.Actor == InActor; });

	if(LifetimeData && ensure(UserGUID.IsValid()))
	{
		UE_LOG(LogCadence, Log, TEXT("UnregisterActorUsage: Actor: %s GUID: %s"), *InActor->GetName(), *UserGUID.ToString());
		if(LifetimeData->UsageGUIDs.RemoveSingle(UserGUID) == 0)
		{
			//TODO: Fix getter/setters to appropriately persist their GUIDs somehow? It seems to cause this error firing in some way
			UE_LOG(LogCadence, Error, TEXT("Cannot remove GUID that isn't in list!"));
		}

		if(LifetimeData->Lifetime == ECadenceActorLifetime::Automatic && LifetimeData->UsageGUIDs.Num() == 0)
		{			
			UE_LOG(LogCadence, Log, TEXT("UnregisterActorUsage.Destroy: Actor: %s"), *InActor->GetName());
			LifetimeData->Actor->Destroy();
			Data.RemoveSingle(*LifetimeData);			
		}
	}
}

TArray<FGuid> UCadenceActorLifetimeManager::GetAutomaticDestroyNodeGUIDs(UCadenceGraphNode* InNode, AActor* InActor)
{
	TArray<FGuid> GUIDs;

	UCadenceGraph* Graph = InNode->GetParentGraph();
	TArray<TObjectPtr<UCadenceGraphNode>> RootExecNodes = Graph->GetRootExecNodesThatLeadToNode(InNode);
	int NumRootPathways = RootExecNodes.Num();

	TArray<TObjectPtr<UCadenceGraphNode>> EndPointNodes;
	GatherEndPointNodes(EndPointNodes, InNode);

	for(int32 PathwayIndex = 0; PathwayIndex < NumRootPathways; ++PathwayIndex)
	{
		for(UCadenceGraphNode* EndNode : EndPointNodes)
		{
			GUIDs.Add(EndNode->GetGUID());
		}
	}
	
	return GUIDs;
}

void UCadenceActorLifetimeManager::GatherEndPointNodes(TArray<TObjectPtr<UCadenceGraphNode>>& InEndPointNodeList, UCadenceGraphNode* InNode) const
{
}
