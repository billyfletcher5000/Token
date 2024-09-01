// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cadence.h"

DEFINE_LOG_CATEGORY(LogCadence);
#define LOCTEXT_NAMESPACE "FCadenceModule"

void FCadenceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCadenceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCadenceModule, Cadence)