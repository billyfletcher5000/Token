// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceTransition.generated.h"

class UCadenceTickableAction;
/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceTransition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	//UCadenceTickableAction* CreateTickable()
};
