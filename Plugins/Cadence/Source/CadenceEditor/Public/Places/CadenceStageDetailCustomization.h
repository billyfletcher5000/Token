// Copyright (C) Billy Fletcher 2025

#pragma once

#include "IDetailCustomization.h"

class ACadenceStageActor;

class FCadenceStageDetailCustomization : public IDetailCustomization
{
public:	
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FReply OnTakeSnapshot(TWeakObjectPtr<ACadenceStageActor> InActor);
};