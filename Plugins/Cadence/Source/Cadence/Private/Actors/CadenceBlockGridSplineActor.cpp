// Copyright (C) Billy Fletcher 2025


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

