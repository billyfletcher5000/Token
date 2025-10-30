// Copyright (C) Billy Fletcher 2025


#include "CadenceAsset.h"

#include "Graph/CadenceGraph.h"
#include "UObject/ObjectSaveContext.h"

void UCadenceAsset::CreateGraph()
{	
	Modify();
	Graph = NewObject<UCadenceGraph>(this);
	Graph->SetFlags(RF_Transactional);
}

void UCadenceAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
	OnPreSaveDelegate.Broadcast();
}
