// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceEditorModule.h"

class FCadenceVariableInlineWidgetFunctions
{
public:
	static void RegisterAll(VariableInlineWidgetFuncMap& VariableToInlineWidgetFunc);
	static TSharedRef<SWidget> CreateInlineEditWidgetNoDefaultValue(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetInt(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetFloat(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetDouble(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetBool(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetVector(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);
	static TSharedRef<SWidget> CreateInlineEditWidgetVector2D(UCadenceVariable* InVariable, TSharedPtr<IPropertyHandle> InPropertyHandle);

protected:
	template<typename ValueType, typename VariableType>
	static void SetVariableValue(ValueType NewValue, VariableType* Variable)
	{
		Variable->Modify();
		Variable->SetValue(NewValue);	
	}

	template<typename ValueType, typename VariableType>
	static ValueType GetVariableValue(VariableType* Variable)
	{
		return Variable->GetValue();
	}
	
	template<typename ValueType, typename VariableType>
	static TOptional<ValueType> GetVariableValueOptional(VariableType* Variable)
	{
		return Variable->GetValue();
	}
	
	template<typename VectorValueType, typename VariableType>
	static TOptional<VectorValueType> GetVectorValueX(VariableType* Variable)
	{
		return Variable->GetValue().X;
	}
	
	template<typename VectorValueType, typename VariableType>
	static TOptional<VectorValueType> GetVectorValueY(VariableType* Variable)
	{
		return Variable->GetValue().Y;
	}

	template<typename VectorValueType, typename VariableType>
	static TOptional<VectorValueType> GetVectorValueZ(VariableType* Variable)
	{
		return Variable->GetValue().Z;
	}

	template<typename VectorType, typename VectorValueType, typename VariableType>
	static void SetVectorValueX(VectorValueType NewValue, VariableType* Variable)
	{
		Variable->Modify();
		VectorType Vector = Variable->GetValue();
		Vector.X = NewValue;
		Variable->SetValue(Vector);
	}

	template<typename VectorType, typename VectorValueType, typename VariableType>
	static void SetVectorValueY(VectorValueType NewValue, VariableType* Variable)
	{
		Variable->Modify();
		VectorType Vector = Variable->GetValue();
		Vector.Y = NewValue;
		Variable->SetValue(Vector);
	}

	template<typename VectorType, typename VectorValueType, typename VariableType>
	static void SetVectorValueZ(VectorValueType NewValue, VariableType* Variable)
	{
		Variable->Modify();
		VectorType Vector = Variable->GetValue();
		Vector.Z = NewValue;
		Variable->SetValue(Vector);
	}
	
	template<typename VariableType>
	static void SetVariableValueCheckBoxState(ECheckBoxState NewValue, VariableType* Variable)
	{
		Variable->Modify();
		Variable->SetValue(NewValue == ECheckBoxState::Checked ? true : false);	
	}

	template<typename VariableType>
	static ECheckBoxState GetVariableValueCheckBoxState(VariableType* Variable)
	{
		return Variable->GetValue() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
};
