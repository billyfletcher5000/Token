// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadencePlaces.generated.h"

USTRUCT()
struct FCadencePlace
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bIncludeLocation = true;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bIncludeLocation"))
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bIncludeRotation = true;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bIncludeRotation"))
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	bool bIncludeScale = true;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bIncludeScale"))
	FVector Scale = FVector::OneVector;
};

USTRUCT()
struct FCadenceComponentPlacePair
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FCadencePlace Place;

	UPROPERTY(VisibleAnywhere)
	FString ComponentPathName;
};

USTRUCT()
struct FCadenceActorPlaceSet
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FCadencePlace Place;

	UPROPERTY(VisibleAnywhere)
	FGuid TrackedActorID;

	UPROPERTY(VisibleAnywhere)
	TArray<FCadenceComponentPlacePair> ComponentPlacePairs;
};

UCLASS()
class CADENCE_API UCadencePlacesSnapshot : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly)
	FVector StageSourceLocation;
	
	UPROPERTY(VisibleInstanceOnly)
	TArray<FCadenceActorPlaceSet> PlaceSets;
};

UCLASS()
class CADENCE_API ACadenceStageActor : public AVolume
{
	GENERATED_BODY()

public:	
	UCadencePlacesSnapshot* TakePlacesSnapshot();
	
public:
	UPROPERTY(EditAnywhere, Category="Stage")
	bool bUnlimitedBounds = false;

	UPROPERTY(EditAnywhere, Category="Stage")
	TArray<int32> TrackingChannels;

	UPROPERTY(EditAnywhere, Category="Stage")
	bool bIncludeLocation = true;

	UPROPERTY(EditAnywhere, Category="Stage")
	bool bIncludeRotation = true;

	UPROPERTY(EditAnywhere, Category="Stage")
	bool bIncludeScale = true;
};