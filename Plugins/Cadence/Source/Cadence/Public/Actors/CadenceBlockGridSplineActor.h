// Fill out your copyright notice in the Description page of Project Settings.

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

protected:
	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	FVector2D SingleGridUnitScale = FVector2D::One();

	UPROPERTY()
	float GridBlockWidth = 1.0f;
};
