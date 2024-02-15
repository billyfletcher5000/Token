// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFactoryFunctionLibrary.h"

#include "CustomBlueprintFactory.h"

UFactory* UBlueprintFactoryFunctionLibrary::CreateBlueprintFactory(const TSubclassOf<UObject> BPClass)
{
	UCustomBlueprintFactory* CustomBPFactory = NewObject<UCustomBlueprintFactory>();

	CustomBPFactory->SupportedClass = BPClass;
	CustomBPFactory->ParentClass = BPClass;

	return CustomBPFactory;
}
