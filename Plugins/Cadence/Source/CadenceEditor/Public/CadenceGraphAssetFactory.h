// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CadenceGraphAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceGraphAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCadenceGraphAssetFactory(const FObjectInitializer& ObjectInitializer);

public: // UFactory
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
};
