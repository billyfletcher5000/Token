#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FCadenceGraphAssetAction : public FAssetTypeActions_Base
{
public:
	FCadenceGraphAssetAction(EAssetTypeCategories::Type Category);

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