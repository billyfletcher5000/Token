// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGridPreviewDrawCommand.generated.h"

#if WITH_EDITOR

USTRUCT()
struct CADENCE_API FCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()
	
	FLinearColor Color = FLinearColor::Blue;
};

USTRUCT()
struct CADENCE_API FCadenceGridPreviewDrawPointCommand : public FCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()
	
	// Position in relative/0-1 space
	FVector2D Position;

	// Size in pixels
	float Size = 3.0f;
};

USTRUCT()
struct CADENCE_API FCadenceGridPreviewDrawLineCommand : public FCadenceGridPreviewDrawCommand
{
	GENERATED_BODY()
	
	// Position in relative/0-1 space
	FVector2D PositionStart;

	// Position in relative/0-1 space
	FVector2D PositionEnd;

	// Thickness in pixels
	float Thickness = 2.0f;
};

USTRUCT()
struct CADENCE_API FCadenceGridPreviewDrawBoxCommand : public FCadenceGridPreviewDrawCommand
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

USTRUCT()
struct CADENCE_API FCadenceGridPreviewDrawCircleCommand : public FCadenceGridPreviewDrawCommand
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