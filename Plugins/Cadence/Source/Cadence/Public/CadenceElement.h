// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CADENCE_API ICadenceElement
{
public:
	virtual ~ICadenceElement() = default;
	virtual void PreExecute() = 0;
	
};
