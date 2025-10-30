// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class UCadenceSequencerSection;
class UCadenceGraph;
class UCadenceSequencerTrack;

class FCadenceSequencerSectionNameEditCustomization : public IPropertyTypeCustomization
{
public:
	FCadenceSequencerSectionNameEditCustomization(UCadenceSequencerTrack* InParentTrack) : ParentTrack(InParentTrack) {}
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(UCadenceSequencerTrack* InParentTrack);
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:	
	TObjectPtr<UCadenceSequencerTrack> ParentTrack;	
	TSharedPtr<IPropertyHandle> NameHandle;
	FString CurrentValue;
};

class FCadenceSequencerSectionNameSelectCustomization : public IPropertyTypeCustomization
{
public:
	FCadenceSequencerSectionNameSelectCustomization(UCadenceGraph* InGraph) : ParentGraph(InGraph) {}
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(UCadenceGraph* InGraph);
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:	
	TObjectPtr<UCadenceGraph> ParentGraph;
	TArray<UCadenceSequencerSection*> Sections;
	TSharedPtr<IPropertyHandle> NameHandle;
};

