// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Cadence.h"
#include "CadenceContext.h"
#include "Actors/CadenceActorLifetime.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"

#include "CadenceActorNode.generated.h"

UCLASS(Abstract)
class CADENCE_API UCadenceActorNode : public UCadenceGraphNode
{
	GENERATED_BODY()

protected:
	template<typename T>
	T* SpawnActor(UCadenceContext* Context, UCadenceGraphNodePin* OutputPin, FVector const& Location = FVector::ZeroVector, FRotator const& Rotation = FRotator::ZeroRotator, FActorSpawnParameters SpawnParameters = FActorSpawnParameters())
	{		
		return SpawnActor<T>(T::StaticClass(), Context, OutputPin, Location, Rotation, SpawnParameters);
	}

	template<typename T>
	T* SpawnActor(UClass* InClass, UCadenceContext* Context, UCadenceGraphNodePin* OutputPin, FVector const& Location = FVector::ZeroVector, FRotator const& Rotation = FRotator::ZeroRotator, FActorSpawnParameters SpawnParameters = FActorSpawnParameters())
	{
		AActor* Actor = GetWorld()->SpawnActor(InClass, &Location, &Rotation, SpawnParameters);

		FGuid TempGUID = OutputPin->GetVariable()->GetGUID();
		UE_LOG(LogCadence, Log, TEXT("SpawnActor Register: %s - %s"), *OutputPin->GetParentNode()->GetName(), *TempGUID.ToString());
		
		Context->ActorLifetimeManager->RegisterActor(Actor, Lifetime, OutputPin->GetVariable()->GetGUID());
		
		return CastChecked<T>(Actor);
	}
	
protected:
	UPROPERTY(EditAnywhere)
	ECadenceActorLifetime Lifetime = ECadenceActorLifetime::Automatic;
};

