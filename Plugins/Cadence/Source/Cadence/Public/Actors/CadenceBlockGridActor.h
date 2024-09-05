// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CadenceBlockGridActor.generated.h"

class UNiagaraSystem;

UCLASS()
class CADENCE_API ACadenceBlockGridActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACadenceBlockGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Blueprintable, BlueprintPure)
	FVector ProjectOnToGrid(const FVector& InWorldVector) const;

	UFUNCTION(Blueprintable, BlueprintPure)
	FVector GridPositionToLocalLocation(const FVector2D& InGridPosition) const;

	UFUNCTION(Blueprintable, BlueprintPure)
	FVector GridPositionToWorldLocation(const FVector2D& InGridPosition) const;

	UFUNCTION(Blueprintable, BlueprintPure)
	FVector2D WorldLocationToGridPosition(const FVector& InWorldPosition, const bool bInSnapToPixel = true) const;

protected:
	UPROPERTY(EditAnywhere, Category="Block Grid")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, Category="Block Grid")
	FVector2D GridTotalSize = FVector2D(64.0f,64.0f);

	UPROPERTY(EditAnywhere, Category="Block Grid")
	FVector2D GridVisualScale = FVector2D(16.0f,16.0f);

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
};
