#pragma once

#include "IPropertyTypeCustomization.h"

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
	
private:
	FName CurrentItem;
	TArray<FName> VariableTypeNames;
	TMap<FName, UClass*> VariableTypeNameToClass;
};
