#include "CadenceGraphPropertyCustomization.h"

#include "CadenceEditor.h"
#include "CadenceGraph.h"
#include "CadenceGraphNode.h"
#include "CadenceUserVariableNodes.h"
#include "CadenceVariable.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Logging/StructuredLog.h"
#include "Input/Reply.h"

DEFINE_LOG_CATEGORY(LogCadence);

const FString FCadenceGraphUserVariableSetCustomization::DefaultVariableNameBase = TEXT("NewVar"); 

FCadenceGraphUserVariableSetCustomization::FCadenceGraphUserVariableSetCustomization()
{
	GenerateVariableLists();
	CurrentItem = VariableTypeNames.Num() > 0 ? VariableTypeNames[0] : TEXT("Invalid");
}

TSharedRef<IPropertyTypeCustomization> FCadenceGraphUserVariableSetCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceGraphUserVariableSetCustomization());
}

void FCadenceGraphUserVariableSetCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
                                                                FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow.NameContent()
				 [
					 SNew(STextBlock)
					 .Text(FText::FromString(TEXT("User Variables")))
					 .Font(IDetailLayoutBuilder::GetDetailFont())
				 ];
}

void FCadenceGraphUserVariableSetCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if(!PropertyHandle->IsValidHandle())
		return;

	TSharedPtr<IPropertyHandle> VariablesPropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCadenceGraphUserVariableSet, Variables));
	TSharedPtr<IPropertyHandleArray> PropertyArray = VariablesPropHandle->AsArray();
		
	auto OnAddClicked = [&ChildBuilder, PropertyArray, this]
	{
		if(PropertyArray->AddItem() == FPropertyAccess::Success)
		{
			uint32 NumElements = 0;
			PropertyArray->GetNumElements(NumElements);

			TSharedRef<IPropertyHandle> NewElement = PropertyArray->GetElement(NumElements - 1);
			auto VariableProperty = NewElement->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCadenceNamedVariable, Variable));

			TArray<UObject*> ElementObjects;
			VariableProperty->GetOuterObjects(ElementObjects);
			UCadenceGraph* Graph = Cast<UCadenceGraph>(ElementObjects[0]);
			
			UClass* VariableType = VariableTypeNameToClass[CurrentItem];
			UCadenceVariable* NewVariable = NewObject<UCadenceVariable>(Graph, VariableType);
			NewVariable->SetUserVariableName(GetUniqueDefaultVariableName(Graph->UserVariables.Variables));
			VariableProperty->SetValue(NewVariable);

			ChildBuilder.GetParentCategory().GetParentLayout().ForceRefreshDetails();			
		}
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
				 ]
				 .ValueContent()
				 [
				 	SNew(SHorizontalBox)
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

	uint32 NumElements = 0;
	if(PropertyArray->GetNumElements(NumElements))
	{
		for(uint32 ElementIndex = 0; ElementIndex < NumElements; ++ElementIndex)
		{
			TSharedRef<IPropertyHandle> ElementProperty = PropertyArray->GetElement(ElementIndex);
			ChildBuilder.AddProperty(ElementProperty);
		}
	}	
}

TSharedRef<SWidget> FCadenceGraphUserVariableSetCustomization::MakeWidgetForOption(FName InOption)
{
	return SNew(STextBlock).Text(FText::FromString(InOption.ToString()));
}

void FCadenceGraphUserVariableSetCustomization::OnSelectionChanged(FName NewValue, ESelectInfo::Type)
{
	CurrentItem = NewValue;
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
		
		if(VariableCDO->IsArray())
		{
			VariableDisplayName = FName(VariableDisplayName.ToString() + "Array");
		}
		
		if(VariableTypeNames.Contains(VariableDisplayName))
		{
			UClass* PreviousVariableType = VariableTypeNameToClass[VariableDisplayName];
			UE_LOGFMT(LogCadence, Warning, "UserVariableSet: Conflicting Variable types detected: Type: {0} | ClassA: {1} | ClassB: {2}", VariableDisplayName.ToString(), VariableType->GetName(), PreviousVariableType->GetName());
		}
		else
		{
			VariableTypeNameToClass.Add(VariableDisplayName, VariableType);
			VariableTypeNames.Add(VariableDisplayName);
		}
	}

	//VariableTypeNames.Sort();	
}

bool FCadenceGraphUserVariableSetCustomization::VariableAlreadyExistsWithName(TArray<FCadenceNamedVariable>& UserVariableArray, const FName& InName)
{
	if(InName == NAME_None)
		return false;
	
	for(FCadenceNamedVariable& Variable : UserVariableArray)
	{
		if(Variable.Variable && Variable.Variable->GetUserVariableName() == InName)
			return true;
	}

	return false;
}

FName FCadenceGraphUserVariableSetCustomization::GetUniqueDefaultVariableName(TArray<FCadenceNamedVariable>& UserVariableArray)
{
	FName CurrentTestName = FName(DefaultVariableNameBase);
	uint32 VariableCopyIndex = 1;
	while(VariableAlreadyExistsWithName(UserVariableArray, CurrentTestName))
	{
		VariableCopyIndex++;
		CurrentTestName = FName(DefaultVariableNameBase + " " + FString::FromInt(VariableCopyIndex));
	}

	return CurrentTestName;
}

TSharedRef<IPropertyTypeCustomization> FCadenceGraphNamedVariableCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceGraphNamedVariableCustomization());
}

void FCadenceGraphNamedVariableCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceGraphNamedVariableCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	TSharedPtr<IPropertyHandle> VariableProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCadenceNamedVariable, Variable));

	UObject* VariableUObject = nullptr;
	VariableProperty->GetValue(VariableUObject);
	UCadenceVariable* Variable = Cast<UCadenceVariable>(VariableUObject);

	TSharedPtr<SWidget> VariablePropDisplay;
	FCadenceEditorModule& CadenceEditorModule = FModuleManager::LoadModuleChecked<FCadenceEditorModule>("CadenceEditor");
	auto VariableClassToWidgetFunctionMap = CadenceEditorModule.VariableToInlineWidgetFunc;
	FName VariableClassName = Variable->GetClass()->GetFName();
	if(VariableClassToWidgetFunctionMap.Contains(VariableClassName))
	{
		auto WidgetFunction = VariableClassToWidgetFunctionMap[VariableClassName];
		VariablePropDisplay = WidgetFunction.Execute(Variable, VariableProperty);
	}
	else
	{
		VariablePropDisplay = SNew(SProperty, VariableProperty)
			 .ShouldDisplayName(false);
	}
	
	ChildBuilder.AddCustomRow( FText::FromString("UserVariableRow"))
				 .NameContent()
				 [
					 SNew(SHorizontalBox)
					 + SHorizontalBox::Slot()
					 .FillWidth(1.0f)
					 .MaxWidth(32.0f)
					 [
						 SNew(SButton)
						 .ButtonStyle(FAppStyle::Get(), "SimpleButton")
						 .HAlign(HAlign_Left)
						 .OnClicked_Static(&FCadenceGraphNamedVariableCustomization::OnDeleteButtonPressed, PropertyHandle.ToSharedPtr(), &ChildBuilder, Variable)
						 [							 
						 	SNew(SImage)
							 .Image(FAppStyle::Get().GetBrush("Icons.Delete"))
							 .ColorAndOpacity(FSlateColor::UseForeground())
						 ]
					 ]
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
						 SNew(SEditableTextBox)
						 .Text_Static(&FCadenceGraphNamedVariableCustomization::GetVariableText, Variable)
						 .OnTextCommitted_Static(&FCadenceGraphNamedVariableCustomization::OnVariableTextCommitted, Variable)
					 ]
				 ]
				 .ValueContent()
				 [
					 SNew(SHorizontalBox)
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
					 	VariablePropDisplay.ToSharedRef()
					 ]
				 ];
}

FText FCadenceGraphNamedVariableCustomization::GetVariableText(UCadenceVariable* Variable)
{
	return FText::FromName(Variable->GetUserVariableName());
}

void FCadenceGraphNamedVariableCustomization::OnVariableTextCommitted(const FText& InText, ETextCommit::Type CommitType, UCadenceVariable* Variable)
{
	Variable->SetUserVariableName(FName(InText.ToString()));
}

FReply FCadenceGraphNamedVariableCustomization::OnDeleteButtonPressed(TSharedPtr<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder* ChildBuilder, UCadenceVariable* Variable)
{
	auto ParentHandle = PropertyHandle->GetParentHandle();
	auto ParentArrayHandle = ParentHandle->AsArray();

	int32 ArrayIndex = PropertyHandle->GetIndexInArray();
	if(ArrayIndex != INDEX_NONE)
	{		
		ParentArrayHandle->DeleteItem(ArrayIndex);
		ChildBuilder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}