// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CadenceBlockGridActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneCaptureComponent2D;

UCLASS()
class CADENCE_API ACadenceBlockGridActor : public AActor
{
	GENERATED_BODY()

public:
	ACadenceBlockGridActor();

protected:
	// AActor
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// AActor End
	
	virtual void UpdateEditorValues(); 	
	
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

	UPROPERTY(EditAnywhere, Category="Block Grid")
	TObjectPtr<UTextureRenderTarget2D> SceneCaptureRenderTexture;
	
	UPROPERTY(EditAnywhere, Category="Block Grid")
	TObjectPtr<UMaterial> SceneCapturePostProcessMaterial;
	
	UPROPERTY(EditAnywhere, Category="Block Grid", AdvancedDisplay)
	FName GridTotalSizeVariableName = "User.GridSize";

	UPROPERTY(EditAnywhere, Category="Block Grid", AdvancedDisplay)
	FName GridVisualScaleVariableName = "User.GridScale";
	
	UPROPERTY(EditAnywhere, Category="Block Grid", AdvancedDisplay)
	FName RenderTextureVariableName = "User.RenderTexture";

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent2D;
};
