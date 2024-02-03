// Fill out your copyright notice in the Description page of Project Settings.


#include "AGridWriter.h"

// Sets default values
AAGridWriter::AAGridWriter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAGridWriter::BeginPlay()
{
	Super::BeginPlay();

	DynamicTexture = NewObject<UDynamicTexture>();
	DynamicTexture->Initialize(TextureSize.X, TextureSize.Y, FLinearColor::Black, TF_Nearest);

	
	//TargetMaterial->sette
}

// Called every frame
void AAGridWriter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	double sinTime = sin(GetWorld()->GetTimeSeconds());
	uint8 hue = (uint8)(sinTime * 255.0);
	FLinearColor color = FLinearColor::MakeFromHSV8(hue, 255, 255);
	DynamicTexture->Fill(color);
	DynamicTexture->UpdateTexture();
}

