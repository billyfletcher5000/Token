// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGridPreviewDrawCommand.generated.h"

#if WITH_EDITOR

UCLASS(Abstract)
class CADENCE_API UCadenceGridPreviewDrawCommand : public UObject
{
	GENERATED_BODY()

public:
	FLinearColor Color = FLinearColor::Blue;
};

UCLASS()
class CADENCE_API UCadenceGridPreviewDrawPointCommand : public UCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()
public:
	// Position in relative/0-1 space
	FVector2D Position;

	// Size
	float Size = 3.0f;
};

UCLASS()
class CADENCE_API UCadenceGridPreviewDrawLineCommand : public UCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()

public:	
	// Position in relative/0-1 space
	FVector2D PositionStart;

	// Position in relative/0-1 space
	FVector2D PositionEnd;

	// Thickness in pixels
	float Thickness = 2.0f;
};

UCLASS()
class CADENCE_API UCadenceGridPreviewDrawBoxCommand : public UCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()

	// Position in relative/0-1 space
	FVector2D Position;

	// Extents in relative/0-1 space
	FVector2D Extents;

	// Thickness in pixels
	float BorderThickness = 2.0f;

	FLinearColor BorderColor;
	
	bool bIsFilled = true;
};

UCLASS()
class CADENCE_API UCadenceGridPreviewDrawCircleCommand : public UCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()
	
	// Position in relative/0-1 space
	FVector2D Position;

	// Radius in relative/0-1 space
	float Radius = 0.2f;

	// Thickness in pixels
	float Thickness = 2.0f;
};

#endif