// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceReactionGroupFactory.generated.h"

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceReactionGroupFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCadenceReactionGroupFactory(const FObjectInitializer& ObjectInitializer);

public: // UFactory
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
};
