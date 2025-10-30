// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "UObject/Object.h"

class FCadenceRhythmTrackMidiAssetAction : public FAssetTypeActions_Base
{
public:
	FCadenceRhythmTrackMidiAssetAction(EAssetTypeCategories::Type Category);

public: // FAssetTypeActions_Base
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type AssetCategory;
	
public:
	static const FText AssetName;
	static const FColor TypeColor;
};
