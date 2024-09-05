// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceBlockGridActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

// Sets default values
ACadenceBlockGridActor::ACadenceBlockGridActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraBlockGridComponent");
}

// Called when the game starts or when spawned
void ACadenceBlockGridActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACadenceBlockGridActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(NiagaraSystem && NiagaraComponent)
	{
		NiagaraComponent->SetAsset(NiagaraSystem);
	}
}

// Called every frame
void ACadenceBlockGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ACadenceBlockGridActor::ProjectOnToGrid(const FVector& InWorldVector) const
{
	FVector ActorLocation = GetActorLocation();
	FVector ActorForwardVector = GetActorForwardVector();
	
	return ActorLocation + FVector::VectorPlaneProject(InWorldVector - ActorLocation, ActorForwardVector); 
}

FVector ACadenceBlockGridActor::GridPositionToLocalLocation(const FVector2D& InGridPosition) const
{
	FVector2D LocalPosition3D = (InGridPosition - (GridTotalSize * 0.5f)) * GridVisualScale;
	return FVector(0.0f, LocalPosition3D.X, LocalPosition3D.Y);
}

FVector ACadenceBlockGridActor::GridPositionToWorldLocation(const FVector2D& InGridPosition) const
{
	const FTransform& ActorTransform = GetActorTransform();
	return ActorTransform.TransformPosition(GridPositionToLocalLocation(InGridPosition));
}

FVector2D ACadenceBlockGridActor::WorldLocationToGridPosition(const FVector& InWorldPosition, const bool bInSnapToPixel) const
{
	const FTransform& ActorTransform = GetActorTransform();
	
	FVector ProjectedInWorldPosition = ProjectOnToGrid(InWorldPosition);
	FVector LocalInPosition = ActorTransform.InverseTransformPosition(ProjectedInWorldPosition);
	
	FVector RelativePosition = LocalInPosition - GridPositionToLocalLocation(FVector2D::ZeroVector);
	FVector2D ScaledGridPosition(RelativePosition.Y, RelativePosition.Z);
	
	return ScaledGridPosition / GridVisualScale;
}

