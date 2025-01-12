// Fill out your copyright notice in the Description page of Project Settings.


#include "Reaction/CadenceReactionGroupFactory.h"

#include "Reaction/CadenceReactionGroup.h"

UCadenceReactionGroupFactory::UCadenceReactionGroupFactory(const FObjectInitializer& ObjectInitializer)
{	
	SupportedClass = UCadenceReactionGroup::StaticClass();
}

UObject* UCadenceReactionGroupFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCadenceReactionGroup>(InParent, InName, Flags);
}

bool UCadenceReactionGroupFactory::CanCreateNew() const
{
	return true;
}
