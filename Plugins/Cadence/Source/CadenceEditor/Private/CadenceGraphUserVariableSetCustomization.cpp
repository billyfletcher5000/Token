#include "CadenceGraphUserVariableSetCustomization.h"

#include "CadenceVariable.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SListViewSelectorDropdownMenu.h"

FCadenceGraphUserVariableSetCustomization::FCadenceGraphUserVariableSetCustomization()
{
	GenerateVariableLists();
	CurrentItem = VariableTypeNames.Num() > 0 ? VariableTypeNames[0] : TEXT("Invalid");
}

void FCadenceGraphUserVariableSetCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
                                                                FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceGraphUserVariableSetCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if(!PropertyHandle->IsValidHandle())
		return;
	
	ChildBuilder.AddCustomRow( FText::FromString("UserVariablesLabelRow"))
				 .NameContent()
				 [
					 SNew(STextBlock)
					 .Text(FText::FromString(TEXT("User Variables")))
					 .Font(IDetailLayoutBuilder::GetDetailFont())
				 ];

	auto OnAddClicked = [&]
	{
		return FReply::Handled();
	};

	ChildBuilder.AddCustomRow( FText::FromString("UserVariablesLabelRow"))
				 .NameContent()
				 [
					 SNew(SHorizontalBox)
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
						 SNew(SButton)
						 .Text(FText::FromString(TEXT("Add")))
						 .OnClicked_Lambda(OnAddClicked)						 
					 ]
				 	 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
						 SNew(SComboBox<FName>)
						 .OptionsSource(&VariableTypeNames)
					 	 .OnSelectionChanged(this, &FCadenceGraphUserVariableSetCustomization::OnSelectionChanged)
						 .OnGenerateWidget(this, &FCadenceGraphUserVariableSetCustomization::MakeWidgetForOption)
						 .InitiallySelectedItem(CurrentItem)
						 [
						 	SNew(STextBlock)
							.Text(this, &FCadenceGraphUserVariableSetCustomization::GetCurrentItemLabel)
						 ]
					 ]
				 ];
	}

TSharedRef<SWidget> FCadenceGraphUserVariableSetCustomization::MakeWidgetForOption(FName InOption)
{
	return SNew(STextBlock).Text(FText::FromString(InOption.ToString()));
}

void FCadenceGraphUserVariableSetCustomization::OnSelectionChanged(FName NewValue, ESelectInfo::Type)
{
	CurrentItem = NewValue;
}

TSharedRef<IPropertyTypeCustomization> FCadenceGraphUserVariableSetCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceGraphUserVariableSetCustomization());
}

void FCadenceGraphUserVariableSetCustomization::GenerateVariableLists()
{
	VariableTypeNames.Empty();
	VariableTypeNameToClass.Empty();
	
	TArray<TObjectPtr<UClass>> ValidVariableTypes;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceVariable::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			ValidVariableTypes.Add(Class);
		}
	}

	for(TObjectPtr<UClass> VariableType : ValidVariableTypes)
	{
		UCadenceVariable* VariableCDO = VariableType->GetDefaultObject<UCadenceVariable>();
		FName VariableDisplayName = VariableCDO->GetDisplayName();
		if(VariableTypeNames.Contains(VariableDisplayName))
		{
			//Log warning	
		}
		else
		{
			VariableTypeNameToClass.Add(VariableDisplayName, VariableType);
			VariableTypeNames.Add(VariableDisplayName);
		}
	}

	//VariableTypeNames.Sort();	
}
