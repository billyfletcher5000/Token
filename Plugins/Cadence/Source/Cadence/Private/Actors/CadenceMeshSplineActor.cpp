// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceMeshSplineActor.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


// Sets default values
ACadenceMeshSplineActor::ACadenceMeshSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = this->CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);
}

void ACadenceMeshSplineActor::SetSplinePoints(const TArray<FVector>& InPoints, ESplineCoordinateSpace::Type InCoordinateSpace)
{
	SplineComponent->SetSplinePoints(InPoints, InCoordinateSpace);
	UpdateMeshComponents(EComponentCreationMethod::Instance, true);
}

void ACadenceMeshSplineActor::UpdateMeshComponents(const EComponentCreationMethod& CreationMethod, bool bDestroyPrevious)
{
	if(bDestroyPrevious)
	{
		for(USplineMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->DestroyComponent();
		}
	}
	
	MeshComponents.Empty();

	int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
	
	for(int32 CurrentIndex = 0; CurrentIndex < NumPoints - 1; ++CurrentIndex)
	{		
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->CreationMethod = CreationMethod;
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);  
		SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMeshComponent->SetVisibleInSceneCaptureOnly(bVisibleInSceneCaptureOnly);
		SplineMeshComponent->SetRenderCustomDepth(bRenderCustomDepth);
		SplineMeshComponent->SetCustomDepthStencilValue(CustomDepthStencilValue);
		SplineMeshComponent->SetCustomDepthStencilWriteMask(CustomDepthStencilWriteMask);
		
		if(CurrentIndex == 0 && bHasSpecificStartMesh)
		{
			SplineMeshComponent->SetStaticMesh(SplineMeshStart);
		}
		else if(CurrentIndex == NumPoints - 2 && bHasSpecificEndMesh)
		{
			SplineMeshComponent->SetStaticMesh(SplineMeshEnd);
		}
		else
		{
			SplineMeshComponent->SetStaticMesh(SplineMeshMiddle);
		}

		FVector StartPosition = SplineComponent->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::Local);
		FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::Local);
		FVector EndPosition = SplineComponent->GetLocationAtSplinePoint(CurrentIndex + 1, ESplineCoordinateSpace::Local);
		FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(CurrentIndex + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartScale(SplineMeshScale);
		SplineMeshComponent->SetEndScale(SplineMeshScale);
		
		SplineMeshComponent->SetForwardAxis(ForwardAxis);
		SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);

		MeshComponents.Add(SplineMeshComponent);
	}	
}

void ACadenceMeshSplineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMeshComponents(EComponentCreationMethod::UserConstructionScript, false);
}
