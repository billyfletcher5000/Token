// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceAsset.h"

#include "Graph/CadenceGraph.h"
#include "UObject/ObjectSaveContext.h"

void UCadenceAsset::CreateGraph()
{	
	Modify();
	Graph = NewObject<UCadenceGraph>(this);
}

void UCadenceAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
	OnPreSaveDelegate.Broadcast();
}
