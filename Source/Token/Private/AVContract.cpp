// Copyright (C) Billy Fletcher 2025


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
