// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class UCadenceSequencerTrack;

class FCadenceSequencerSectionNameCustomization : public IPropertyTypeCustomization
{
public:
	FCadenceSequencerSectionNameCustomization(UCadenceSequencerTrack* InParentTrack) : ParentTrack(InParentTrack) {}
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(UCadenceSequencerTrack* InParentTrack);
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:	
	TObjectPtr<UCadenceSequencerTrack> ParentTrack;	
	TSharedPtr<IPropertyHandle> NameHandle;
	FString CurrentValue;
};
