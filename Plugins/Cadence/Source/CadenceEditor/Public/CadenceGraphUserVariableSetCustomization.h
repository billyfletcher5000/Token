#pragma once

#include "IPropertyTypeCustomization.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCadence, Log, All);

class FCadenceGraphUserVariableSetCustomization : public IPropertyTypeCustomization
{
public:
	FCadenceGraphUserVariableSetCustomization();
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	FText GetCurrentItemLabel() const { return FText::FromString(CurrentItem.ToString()); }

private:
	TSharedRef<SWidget> MakeWidgetForOption(FName InOption);
	void OnSelectionChanged(FName NewValue, ESelectInfo::Type SelectInfoType);
	
	void GenerateVariableLists();
	bool VariableAlreadyExistsWithName(TSharedPtr<IPropertyHandleArray> VariablesPropertyArray, const uint32& NumElements, const FName& InName);
	FName GetUniqueDefaultVariableName(TSharedPtr<IPropertyHandleArray> VariablesPropertyArray);

private:
	static const FString DefaultVariableNameBase;
	
private:
	FName CurrentItem;
	TArray<FName> VariableTypeNames;
	TMap<FName, UClass*> VariableTypeNameToClass;
};
