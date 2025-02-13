// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceMeshSplineActor.h"

#include "Actors/CadenceTrackedActorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


// Sets default values
ACadenceMeshSplineActor::ACadenceMeshSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SplineComponent = this->CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);
	
	CreateDefaultSubobject<UCadenceTrackedActorComponent>(TEXT("CadenceTrackedActor"));
}

void ACadenceMeshSplineActor::SetSplinePoints(const TArray<FVector>& InPoints, ESplineCoordinateSpace::Type InCoordinateSpace)
{
	SplineComponent->SetSplinePoints(InPoints, InCoordinateSpace);
	UpdatePivot();
	UpdateMeshComponentTransforms();
}

void ACadenceMeshSplineActor::SetSplinePoint(const int32& InPointIndex, const FVector& InPosition, ESplineCoordinateSpace::Type InCoordinateSpace)
{
	SplineComponent->SetLocationAtSplinePoint(InPointIndex, InPosition, InCoordinateSpace);
	UpdatePivot();
	UpdateMeshComponentTransforms();
}

void ACadenceMeshSplineActor::SetPivotType(const ECadenceSplinePivot& InPivotType)
{
	if(PivotType != InPivotType)
	{
		PivotType = InPivotType;
		UpdatePivot();
		UpdateMeshComponentTransforms();
	}
}

void ACadenceMeshSplineActor::SetPivotSpecificPoint(int32 InPivotSpecificPoint)
{
	int32 NewIndex = FMath::Clamp(InPivotSpecificPoint, 0, SplineComponent->GetNumberOfSplinePoints() - 1);
	if(PivotSpecificPointIndex != NewIndex)
	{
		PivotSpecificPointIndex = NewIndex;
		UpdatePivot();
		UpdateMeshComponentTransforms();
	}
}

void ACadenceMeshSplineActor::UpdatePivot()
{
	if(PivotType == ECadenceSplinePivot::Manual)
		return;

	FVector PivotPoint;

	switch (PivotType)
	{
		default:
		case ECadenceSplinePivot::CentreOfPoints:
			{
				FVector Aggreggate = FVector::ZeroVector;

				int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
				for(int32 CurrentIndex = 0; CurrentIndex < NumPoints; ++CurrentIndex)
				{
					Aggreggate += SplineComponent->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
				}

				PivotPoint = Aggreggate / NumPoints;
			}
			break;

		case ECadenceSplinePivot::SpecificPoint:
			{
				PivotPoint = SplineComponent->GetLocationAtSplinePoint(PivotSpecificPointIndex, ESplineCoordinateSpace::World);
			}
			break;
	}
	
	int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
	TArray<FVector> PointWorldPositions;
	PointWorldPositions.SetNumUninitialized(NumPoints);

	for(int32 CurrentIndex = 0; CurrentIndex < NumPoints; ++CurrentIndex)
	{
		PointWorldPositions[CurrentIndex] = SplineComponent->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
	}
	
	SetActorLocation(PivotPoint);
	
	for(int32 CurrentIndex = 0; CurrentIndex < NumPoints; ++CurrentIndex)
	{
		FVector PointPosition = PointWorldPositions[CurrentIndex];
		SplineComponent->SetLocationAtSplinePoint(CurrentIndex, PointPosition, ESplineCoordinateSpace::World);
	}
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

void ACadenceMeshSplineActor::UpdateMeshComponentTransforms()
{
	int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();

	if(MeshComponents.Num() != NumPoints - 1)
	{
		UpdateMeshComponents(EComponentCreationMethod::Instance, true);
		return;
	}
	
	for(int32 CurrentIndex = 0; CurrentIndex < NumPoints - 1; ++CurrentIndex)
	{
		USplineMeshComponent* SplineMeshComponent = MeshComponents[CurrentIndex];

		FVector StartPosition = SplineComponent->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::Local);
		FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::Local);
		FVector EndPosition = SplineComponent->GetLocationAtSplinePoint(CurrentIndex + 1, ESplineCoordinateSpace::Local);
		FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(CurrentIndex + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartScale(SplineMeshScale);
		SplineMeshComponent->SetEndScale(SplineMeshScale);
		
		SplineMeshComponent->SetForwardAxis(ForwardAxis);
		SplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
	}
}

void ACadenceMeshSplineActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMeshComponents(EComponentCreationMethod::UserConstructionScript, false);
}
