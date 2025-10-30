// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FCadenceReactionGroupAssetAction : public FAssetTypeActions_Base
{
public:
	FCadenceReactionGroupAssetAction(EAssetTypeCategories::Type Category);

public: // FAssetTypeActions_Base
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	//virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type AssetCategory;
	
public:
	static const FText AssetName;
	static const FColor TypeColor;
};