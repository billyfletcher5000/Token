// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicTexture.h"
#include "AGridWriter.generated.h"

UCLASS()
class TOKEN_API AAGridWriter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAGridWriter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> TargetMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TextureSize;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TObjectPtr<UDynamicTexture> DynamicTexture;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
