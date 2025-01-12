// Fill out your copyright notice in the Description page of Project Settings.


#include "Reaction/CadenceReactionGroupCustomization.h"

#include "CadenceGraphSchema.h"
#include "CadencePalette.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "ISinglePropertyView.h"
#include "PropertyCustomizationHelpers.h"
#include "Graph/CadenceVariable.h"
#include "Reaction/CadenceReactionGroup.h"

const FString DefaultVariableNameBase = TEXT("NewVar");

namespace CadencReactionCustomizationHelper
{
	bool VariableAlreadyExistsWithName(TArray<TObjectPtr<UCadenceVariable>>& UserVariableArray, const FName& InName)
	{
		if(InName == NAME_None)
			return false;
	
		for(UCadenceVariable* Variable : UserVariableArray)
		{
			if(Variable && Variable->GetUserVariableName() == InName)
				return true;
		}

		return false;
	}

	FName GetUniqueDefaultVariableName(TArray<TObjectPtr<UCadenceVariable>>& UserVariableArray, FName InBaseName = NAME_None)
	{
		FName BaseName = InBaseName == NAME_None ? FName(DefaultVariableNameBase) : InBaseName;
		FName CurrentTestName = BaseName;
		uint32 VariableCopyIndex = 1;
		while(VariableAlreadyExistsWithName(UserVariableArray, CurrentTestName))
		{
			VariableCopyIndex++;
			CurrentTestName = FName(BaseName.ToString() + " " + FString::FromInt(VariableCopyIndex));
		}

		return CurrentTestName;
	}
}

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
	VariablesBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FCadenceReactionGroupCustomization::OnGenerateVariable, Groups[0], &DetailBuilder));

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
	NewVariable->SetUserVariableName(InVar->GetUserVariableName());

	InGroup->Modify();
	InGroup->Variables.Insert(NewVariable, VarIndex);
	InGroup->Variables.Remove(InVar);

	InLayoutBuilder->ForceRefreshDetails();
}

void FCadenceReactionGroupCustomization::OnGenerateVariable(TSharedRef<IPropertyHandle> Property, int32 Index, IDetailChildrenBuilder& ChildrenBuilder, TWeakObjectPtr<UCadenceReactionGroup> InGroup, IDetailLayoutBuilder* InLayoutBuilder)
{
	UObject* CurrentValue = nullptr;
	Property->GetValue(CurrentValue);
	UCadenceVariable* Variable = Cast<UCadenceVariable>(CurrentValue);

	bool bVariableChanged = false;
	if(Variable != nullptr)
	{
		int32 NumExisting = InGroup->Variables.FilterByPredicate([&Variable] (UCadenceVariable* InVar)
		{
			return InVar == Variable;
		}).Num();
		
		if(NumExisting > 1)
		{			
			UCadenceVariable* NewVariable = NewObject<UCadenceVariable>(InGroup.Get(), Variable->GetClass());
			NewVariable->CopyValueFrom(Variable);
			NewVariable->SetUserVariableName(CadencReactionCustomizationHelper::GetUniqueDefaultVariableName(InGroup->Variables, Variable->GetUserVariableName()));
			Variable = NewVariable;
			bVariableChanged = true;
		}
	}		
	else
	{
		Variable = NewObject<UCadenceVariable>(InGroup.Get(), UCadenceVariableBool::StaticClass());
		Variable->SetUserVariableName(CadencReactionCustomizationHelper::GetUniqueDefaultVariableName(InGroup->Variables));
		bVariableChanged = true;
	}

	if(bVariableChanged)
	{
		Property->NotifyPreChange();
		Property->SetValue(Variable);
		Property->NotifyPostChange(EPropertyChangeType::ValueSet);
		Property->NotifyFinishedChangingProperties();
	}

	TSharedPtr<FCadenceVariableAction> Action = MakeShareable(new FCadenceVariableAction(Variable));

	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams Params;
	Params.NamePlacement = EPropertyNamePlacement::Hidden;
	auto NameElement = EditModule.CreateSingleProperty(Variable, FName(TEXT("UserVariableName")), Params);
	auto ValueElement = EditModule.CreateSingleProperty(Variable, FName(TEXT("Value")), Params);
	
	ChildrenBuilder.AddProperty(Property)
	                .CustomWidget()
					.NameContent()
					[			
						Property->CreatePropertyNameWidget()
					]
					.ValueContent()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.MinWidth(200)
						.Padding(0, 0, 10, 0)
						[
							NameElement.ToSharedRef()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCadencePinTypeSelectorHelper, Action.ToWeakPtr(), FChangeVariableTypeDelegate::CreateStatic(&FCadenceReactionGroupCustomization::ChangeVariableType, InGroup, InLayoutBuilder), false)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[				
							ValueElement.ToSharedRef()
						]
					];
}
