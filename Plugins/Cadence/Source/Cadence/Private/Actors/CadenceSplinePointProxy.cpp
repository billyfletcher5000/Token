// Copyright (C) Billy Fletcher 2025


#include "Actors/CadenceSplinePointProxy.h"

#include "Actors/CadenceMeshSplineActor.h"
#include "Actors/CadenceTrackedActorComponent.h"
#include "Internationalization/Internationalization.h"


// Sets default values
ACadenceSplinePointProxy::ACadenceSplinePointProxy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProxySceneComponent = CreateDefaultSubobject<UCadenceSplinePointProxySceneComponent>("ProxySceneComponent");
	
	CreateDefaultSubobject<UCadenceTrackedActorComponent>(TEXT("CadenceTrackedActor"));
}

void ACadenceSplinePointProxy::Init(ACadenceMeshSplineActor* InMeshSplineActor, int32 InPointIndex)
{
	ProxySceneComponent->MeshSplineActor = InMeshSplineActor;
	ProxySceneComponent->PointIndex = InPointIndex;
}

#define LOCTEXT_NAMESPACE "SceneComponent"

bool UCadenceSplinePointProxySceneComponent::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{
	// Copy of Super method, needed because I need to see bMoved's value
	
	// static things can move before they are registered (e.g. immediately after streaming), but not after.
	if (!IsValid(this) || CheckStaticMobilityAndWarn(LOCTEXT("InvalidMove", "move")))
	{
		if (OutHit)
		{
			*OutHit = FHitResult();
		}
		return false;
	}

	// Fill in optional output param. SceneComponent doesn't sweep, so this is just an empty result.
	if (OutHit)
	{
		*OutHit = FHitResult(1.f);
	}

	ConditionalUpdateComponentToWorld();

	// early out for zero case
	if( Delta.IsZero() )
	{
		// Skip if no vector or rotation.
		if (NewRotation.Equals(GetComponentTransform().GetRotation(), SCENECOMPONENT_QUAT_TOLERANCE))
		{
			return true;
		}
	}

	// just teleport, sweep is supported for PrimitiveComponents. This will update child components as well.
	const bool bMoved = InternalSetWorldLocationAndRotation(GetComponentLocation() + Delta, NewRotation, false, Teleport);

	// Only update overlaps if not deferring updates within a scope
	if (bMoved && !IsDeferringMovementUpdates())
	{
		// need to update overlap detection in case PrimitiveComponents are attached.
		UpdateOverlaps();
	}

	if(bMoved && PointIndex != -1)
	{
		MeshSplineActor->SetSplinePoint(PointIndex, GetComponentLocation());
	}

	return true;
}

#undef LOCTEXT_NAMESPACE