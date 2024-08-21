// Fill out your copyright notice in the Description page of Project Settings.


#include "AVContract.h"

void UAVContractInstance::Init(UAVContractDataAsset* InDataAsset)
{
	Data = InDataAsset;
	//AVSystem = InAVSystem;
}

void UAVContractInstance::Start()
{
}

void UAVContractInstance::Tick(double DeltaTime)
{
}


void UAVContractInstance::Complete()
{
	OnContractComplete.Broadcast(this);
}
