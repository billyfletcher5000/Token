// Fill out your copyright notice in the Description page of Project Settings.


#include "Reaction/CadenceReactionGroupCustomization.h"

#include "CadenceGraphSchema.h"
#include "CadencePalette.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Graph/CadenceVariable.h"
#include "Reaction/CadenceReactionGroup.h"

TSharedRef<IDetailCustomization> FCadenceReactionGroupCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceReactionGroupCustomization);
}

void FCadenceReactionGroupCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UCadenceReactionGroup>> Groups = DetailBuilder.GetObjectsOfTypeBeingCustomized<UCadenceReactionGroup>();
	if(Groups.Num() != 1)
		return;
	
	auto VariablesProp = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCadenceReactionGroup, Variables));

	IDetailCategoryBuilder& DataCategory = DetailBuilder.EditCategory("Data");
	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout(UCadenceVariable::StaticClass()->GetFName(),
						  FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCadenceVariablePropertyCustomization::MakeInstance, FChangeVariableTypeDelegate::CreateStatic(&FCadenceReactionGroupCustomization::ChangeVariableType, Groups[0], &DetailBuilder), false));

	TSharedRef<FDetailArrayBuilder> VariablesBuilder = MakeShared<FDetailArrayBuilder>(VariablesProp, true, false, true);
	VariablesBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FCadenceReactionGroupCustomization::OnGenerateVariable, Groups[0]));

	DataCategory.AddCustomBuilder(VariablesBuilder);
}

void FCadenceReactionGroupCustomization::ChangeVariableType(UCadenceVariable* InVar, const FEdGraphPinType& InNewPinType, TWeakObjectPtr<UCadenceReactionGroup> InGroup, IDetailLayoutBuilder* InLayoutBuilder)
{
	int32 VarIndex = InGroup->Variables.IndexOfByKey(InVar);
	if(VarIndex == INDEX_NONE)
		return;

	const UCadenceGraphSchema* Schema = GetDefault<UCadenceGraphSchema>();
	UClass* NewClassType = Schema->GetVariableClassFromPinType(InNewPinType);

	if(InVar->GetClass() == NewClassType)
		return;

	UCadenceVariable* NewVariable = NewObject<UCadenceVariable>(InGroup.Get(), NewClassType);

	InGroup->Modify();
	InGroup->Variables.Insert(NewVariable, VarIndex);
	InGroup->Variables.Remove(InVar);

	InLayoutBuilder->ForceRefreshDetails();
}

void FCadenceReactionGroupCustomization::OnGenerateVariable(TSharedRef<IPropertyHandle> Property, int32 Index, IDetailChildrenBuilder& ChildrenBuilder, TWeakObjectPtr<UCadenceReactionGroup> InGroup)
{
	UObject* CurrentValue = nullptr;
	Property->GetValue(CurrentValue);
	
	if(CurrentValue == nullptr)
	{
		UCadenceVariable* Variable = NewObject<UCadenceVariable>(InGroup.Get(), UCadenceVariableBool::StaticClass());
		Property->NotifyPreChange();
		Property->SetValue(Variable);
		Property->NotifyPostChange(EPropertyChangeType::ValueSet);
		Property->NotifyFinishedChangingProperties();
	}	

	IDetailPropertyRow& PropertyRow = ChildrenBuilder.AddProperty(Property);
	PropertyRow.ShowPropertyButtons(false);
	PropertyRow.ShouldAutoExpand(false);

	TSharedPtr<SWidget> NameWidget;
	TSharedPtr<SWidget> ValueWidget;
	PropertyRow.GetDefaultWidgets( NameWidget, ValueWidget);
	PropertyRow.CustomWidget(true)
	.NameContent()
	.HAlign(HAlign_Fill)
	[
		NameWidget.ToSharedRef()
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		ValueWidget.ToSharedRef()
	];
}
