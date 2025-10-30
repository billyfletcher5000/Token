// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceMeshSplineActor.h"
#include "CadenceBlockGridSplineActor.generated.h"

UCLASS()
class CADENCE_API ACadenceBlockGridSplineActor : public ACadenceMeshSplineActor
{
	GENERATED_BODY()

public:
	ACadenceBlockGridSplineActor();

	void SetGridBlockWidth(float InGridBlockWidth);
	float GetGridBlockWidth() const { return GridBlockWidth; }

protected:
	virtual void UpdateMeshComponents(const EComponentCreationMethod& CreationMethod, bool bDestroyPrevious) override;
	virtual void UpdateMeshComponentTransforms() override;

protected:
	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	FVector2D SingleGridUnitScale = FVector2D::One();

	UPROPERTY()
	float GridBlockWidth = 1.0f;
};
