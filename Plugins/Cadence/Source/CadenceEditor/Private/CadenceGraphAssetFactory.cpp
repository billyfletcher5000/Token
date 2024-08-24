// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphAssetFactory.h"

#include "CadenceGraph.h"

UCadenceGraphAssetFactory::UCadenceGraphAssetFactory(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	SupportedClass = UCadenceGraph::StaticClass();
}

UObject* UCadenceGraphAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCadenceGraph>(InParent, InName, Flags);	
}

bool UCadenceGraphAssetFactory::CanCreateNew() const
{
	return true;
}
