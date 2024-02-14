#pragma once
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FTokenBlueprintModule : public IModuleInterface
{
    public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};