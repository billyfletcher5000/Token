// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CadenceSplinePointProxy.generated.h"

class ACadenceMeshSplineActor;
class ACadenceBlockGridActor;
class UCadenceSplinePointProxySceneComponent;

UCLASS()
class CADENCE_API ACadenceSplinePointProxy : public AActor
{
	GENERATED_BODY()

public:
	ACadenceSplinePointProxy();

	void Init(ACadenceMeshSplineActor* InMeshSplineActor, int32 InPointIndex);

protected:
	UPROPERTY()
	TObjectPtr<UCadenceSplinePointProxySceneComponent> ProxySceneComponent;
};

UCLASS()
class UCadenceSplinePointProxySceneComponent : public USceneComponent
{
	GENERATED_BODY()
	
protected:
	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* Hit, EMoveComponentFlags MoveFlags, ETeleportType Teleport) override;

protected:
	friend ACadenceSplinePointProxy;

	UPROPERTY()
	TObjectPtr<ACadenceMeshSplineActor> MeshSplineActor;

	UPROPERTY()
	int32 PointIndex = -1;
};