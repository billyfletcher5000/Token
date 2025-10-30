// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/Object.h"

class FCadenceSequenceActorCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
