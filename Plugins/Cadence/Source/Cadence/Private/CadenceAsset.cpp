// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceAsset.h"

#include "CadenceGraph.h"

void UCadenceAsset::CreateGraph()
{	
	Modify();
	Graph = NewObject<UCadenceGraph>(this);
}
