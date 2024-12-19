// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceVariableInlineWidgetFunctions.h"

#include "Graph/CadenceVariable.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SVectorInputBox.h"

void FCadenceVariableInlineWidgetFunctions::RegisterAll(VariableInlineWidgetFuncMap& VariableToInlineWidgetFunc)
{
	VariableToInlineWidgetFunc.Add(UCadenceVariableArray::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetNoDefaultValue));
	VariableToInlineWidgetFunc.Add(UCadenceVariableInt::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetInt));
	VariableToInlineWidgetFunc.Add(UCadenceVariableFloat::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetFloat));
	VariableToInlineWidgetFunc.Add(UCadenceVariableDouble::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetDouble));
	VariableToInlineWidgetFunc.Add(UCadenceVariableBool::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetBool));
	VariableToInlineWidgetFunc.Add(UCadenceVariableVector::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetVector));
	VariableToInlineWidgetFunc.Add(UCadenceVariableVector2D::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetVector2D));
	VariableToInlineWidgetFunc.Add(UCadenceVariableUObject::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetNoDefaultValue));
	VariableToInlineWidgetFunc.Add(UCadenceVariableTrigger::StaticClass()->GetFName(),
	FVariableCreateInlineWidgetFunc::CreateStatic(&FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetNoDefaultValue));
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetNoDefaultValue(
	UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	return SNew(STextBlock).Text(FText::FromString(TEXT("N/A")));
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetInt(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableInt* Variable = Cast<UCadenceVariableInt>(InVariable);

	return SNew(SSpinBox<int32>)
			.Value_Static(&FCadenceVariableInlineWidgetFunctions::GetVariableValue<int32, UCadenceVariableInt>, Variable)
			.OnValueChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVariableValue<int32, UCadenceVariableInt>, Variable);	
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetFloat(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableFloat* Variable = Cast<UCadenceVariableFloat>(InVariable);

	return SNew(SSpinBox<float>)
			.Value_Static(&FCadenceVariableInlineWidgetFunctions::GetVariableValue<float, UCadenceVariableFloat>, Variable)
			.OnValueChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVariableValue<float, UCadenceVariableFloat>, Variable);
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetDouble(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableDouble* Variable = Cast<UCadenceVariableDouble>(InVariable);

	return SNew(SSpinBox<double>)
			.Value_Static(&FCadenceVariableInlineWidgetFunctions::GetVariableValue<double, UCadenceVariableDouble>, Variable)
			.OnValueChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVariableValue<double, UCadenceVariableDouble>, Variable);
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetBool(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableBool* Variable = Cast<UCadenceVariableBool>(InVariable);

	return SNew(SCheckBox)
			.IsChecked_Static(&FCadenceVariableInlineWidgetFunctions::GetVariableValueCheckBoxState, Variable)
			.OnCheckStateChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVariableValueCheckBoxState<UCadenceVariableBool>, Variable);
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetVector(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableVector* Variable = Cast<UCadenceVariableVector>(InVariable);
	
	using SNumericVector3DInputBox = SNumericVectorInputBox<FVector::FReal>;

	return SNew(SNumericVector3DInputBox)
			.X_Static(&FCadenceVariableInlineWidgetFunctions::GetVectorValueX, Variable)
			.OnXChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVectorValueX<FVector,double,UCadenceVariableVector>, Variable)
			.Y_Static(&FCadenceVariableInlineWidgetFunctions::GetVectorValueY, Variable)
			.OnYChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVectorValueY<FVector,double,UCadenceVariableVector>, Variable)
			.Z_Static(&FCadenceVariableInlineWidgetFunctions::GetVectorValueZ, Variable)
			.OnZChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVectorValueZ<FVector,double,UCadenceVariableVector>, Variable);
}

TSharedRef<SWidget> FCadenceVariableInlineWidgetFunctions::CreateInlineEditWidgetVector2D(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	UCadenceVariableVector2D* Variable = Cast<UCadenceVariableVector2D>(InVariable);
	
	using SNumericVector2DInputBox = SNumericVectorInputBox<FVector2D::FReal, FVector2D, 2>;

	return SNew(SNumericVector2DInputBox)
			.X_Static(&FCadenceVariableInlineWidgetFunctions::GetVectorValueX, Variable)
			.OnXChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVectorValueX<FVector2D,double,UCadenceVariableVector2D>, Variable)
			.Y_Static(&FCadenceVariableInlineWidgetFunctions::GetVectorValueY, Variable)
			.OnYChanged_Static(&FCadenceVariableInlineWidgetFunctions::SetVectorValueY<FVector2D,double,UCadenceVariableVector2D>, Variable);
}