// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CadenceBlockGridSplineActor.h"

ACadenceBlockGridSplineActor::ACadenceBlockGridSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACadenceBlockGridSplineActor::SetGridBlockWidth(float InGridBlockWidth)
{
	GridBlockWidth = InGridBlockWidth;
	UpdateMeshComponentTransforms();
}

void ACadenceBlockGridSplineActor::UpdateMeshComponents(const EComponentCreationMethod& CreationMethod,	bool bDestroyPrevious)
{
	SplineMeshScale = GridBlockWidth * SingleGridUnitScale;
	Super::UpdateMeshComponents(CreationMethod, bDestroyPrevious);
}

void ACadenceBlockGridSplineActor::UpdateMeshComponentTransforms()
{
	SplineMeshScale = GridBlockWidth * SingleGridUnitScale;
	Super::UpdateMeshComponentTransforms();
}

