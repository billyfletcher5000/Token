#pragma once

#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"

struct FCadenceNamedVariable;
class UCadenceVariable;
class FReply;

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
	bool VariableAlreadyExistsWithName(TArray<FCadenceNamedVariable>& UserVariableArray, const FName& InName);
	FName GetUniqueDefaultVariableName(TArray<FCadenceNamedVariable>& UserVariableArray);

private:
	static const FString DefaultVariableNameBase;
	
private:
	FName CurrentItem;
	TArray<FName> VariableTypeNames;
	TMap<FName, UClass*> VariableTypeNameToClass;
};

class FCadenceGraphNamedVariableCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

protected:
	static FText GetVariableText(UCadenceVariable* Variable);
	static void OnVariableTextCommitted(const FText& InText, ETextCommit::Type CommitType, UCadenceVariable* Variable);
	static FReply OnDeleteButtonPressed(TSharedPtr<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder* ChildBuilder, UCadenceVariable* Variable);
};