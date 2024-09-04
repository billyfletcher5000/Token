// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"

#include "CadenceMeshSplineActor.generated.h"

class USplineComponent;

UCLASS()
class CADENCE_API ACadenceMeshSplineActor : public AActor
{
	GENERATED_BODY()

public:
	ACadenceMeshSplineActor();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	bool bHasSpecificStartMesh = false;
	
	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	bool bHasSpecificEndMesh = false;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bHasSpecificStartMesh", EditConditionHides), Category="Spline Mesh")
	TObjectPtr<UStaticMesh> SplineMeshStart;

	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	TObjectPtr<UStaticMesh> SplineMeshMiddle;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bHasSpecificEndMesh", EditConditionHides), Category="Spline Mesh")
	TObjectPtr<UStaticMesh> SplineMeshEnd;

	UPROPERTY(EditAnywhere, Category="Spline Mesh")
	FVector2D SplineMeshScale = FVector2D::One();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spline Mesh")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis = ESplineMeshAxis::X;

	UPROPERTY(EditAnywhere, Category="Rendering")
	bool bVisibleInSceneCaptureOnly = true;

	UPROPERTY(EditAnywhere, Category="Rendering")
	bool bRenderCustomDepth = true;

	UPROPERTY(EditAnywhere, Category="Rendering")
	int32 CustomDepthStencilValue = 1;

	UPROPERTY(EditAnywhere, Category="Rendering")
	ERendererStencilMask CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_Default;
};
