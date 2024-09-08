// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceBlockGridActor.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
ACadenceBlockGridActor::ACadenceBlockGridActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(RootSceneComponent);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraBlockGridComponent");
	NiagaraComponent->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureOrtho2D");
	SceneCaptureComponent2D->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SceneCaptureComponent2D->SetRelativeLocation(FVector(110.0f, 0.0f, 0.0f));
	SceneCaptureComponent2D->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	SceneCaptureComponent2D->OrthoWidth = GridTotalSize.Y * GridVisualScale.Y;
	SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent2D->bAutoCalculateOrthoPlanes = false;
	SceneCaptureComponent2D->bUpdateOrthoPlanes = false;
	SceneCaptureComponent2D->MaxViewDistanceOverride = FarClipDistance;
	
	//TODO: Decide if I need one or both or neither
	SceneCaptureComponent2D->bCaptureEveryFrame = true; 
	SceneCaptureComponent2D->bCaptureOnMovement = true;

	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	SceneCaptureComponent2D->CaptureSource = SCS_FinalColorLDR;
}

// Called when the game starts or when spawned
void ACadenceBlockGridActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACadenceBlockGridActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateEditorValues();
}

#if WITH_EDITOR
void ACadenceBlockGridActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateEditorValues();
}
#endif

void ACadenceBlockGridActor::UpdateEditorValues()
{
	if(NiagaraSystem && NiagaraComponent)
	{
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->SetVariableVec2(GridTotalSizeVariableName, GridTotalSize);
		NiagaraComponent->SetVariableVec2(GridVisualScaleVariableName, GridVisualScale);
		NiagaraComponent->SetVariableFloat(BackMovementVariableName, FarClipDistance);

		if(SceneCaptureRenderTexture)
		{
			NiagaraComponent->SetVariableTexture(RenderTextureVariableName, CastChecked<UTexture>(SceneCaptureRenderTexture));
		}
	}
	
	if(SceneCaptureComponent2D)
	{
		SceneCaptureComponent2D->MaxViewDistanceOverride = FarClipDistance;
			
		if(SceneCapturePostProcessMaterial)
		{
			SceneCaptureComponent2D->AddOrUpdateBlendable(SceneCapturePostProcessMaterial, 1.0f);
		}

		if(SceneCaptureRenderTexture)
		{
			SceneCaptureComponent2D->TextureTarget = SceneCaptureRenderTexture;
		}
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

FVector ACadenceBlockGridActor::NormalisedPositionToLocalLocation(const FVector2D& InNormalisedPosition) const
{
	return GridPositionToLocalLocation(InNormalisedPosition * GridTotalSize);
}

FVector ACadenceBlockGridActor::NormalisedPositionToWorldLocation(const FVector2D& InNormalisedPosition) const
{
	return GridPositionToWorldLocation(InNormalisedPosition * GridTotalSize);
}

FVector2D ACadenceBlockGridActor::WorldLocationToNormalisedPosition(const FVector& InWorldPosition,	const bool bInSnapToPixel) const
{
	return WorldLocationToGridPosition(InWorldPosition, bInSnapToPixel) / GridTotalSize;
}

