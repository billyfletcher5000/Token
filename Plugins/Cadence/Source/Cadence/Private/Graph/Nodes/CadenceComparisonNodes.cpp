// Copyright (C) Billy Fletcher 2025


#include "Graph/Nodes/CadenceComparisonNodes.h"

#include "Graph/CadenceVariable.h"

namespace FCadenceComparisonOperationHelper
{
	template<typename TVarA, typename TVal>
	bool PerformGreaterThanOperation(UCadenceVariable* InVariableA, UCadenceVariable* InVariableB, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVarA* VariableB = Cast<TVarA>(InVariableB);
		if(!ensure(VariableB))
			return false;

		TVal ValueA = VariableA->GetValue();
		TVal ValueB = VariableB->GetValue();

		bool bResult = ValueA > ValueB;

		UCadenceVariableBool* ResultVariable = Cast<UCadenceVariableBool>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(bResult);
		return true;
	}

	template<typename TVarA, typename TVal>
	bool PerformGreaterThanOrEqualOperation(UCadenceVariable* InVariableA, UCadenceVariable* InVariableB, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVarA* VariableB = Cast<TVarA>(InVariableB);
		if(!ensure(VariableB))
			return false;

		TVal ValueA = VariableA->GetValue();
		TVal ValueB = VariableB->GetValue();

		bool bResult = ValueA >= ValueB;

		UCadenceVariableBool* ResultVariable = Cast<UCadenceVariableBool>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(bResult);
		return true;
	}

	template<typename TVarA, typename TVal>
	bool PerformLessThanOperation(UCadenceVariable* InVariableA, UCadenceVariable* InVariableB, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVarA* VariableB = Cast<TVarA>(InVariableB);
		if(!ensure(VariableB))
			return false;

		TVal ValueA = VariableA->GetValue();
		TVal ValueB = VariableB->GetValue();

		bool bResult = ValueA < ValueB;

		UCadenceVariableBool* ResultVariable = Cast<UCadenceVariableBool>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(bResult);
		return true;
	}

	template<typename TVarA, typename TVal>
	bool PerformLessThanOrEqualOperation(UCadenceVariable* InVariableA, UCadenceVariable* InVariableB, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVarA* VariableB = Cast<TVarA>(InVariableB);
		if(!ensure(VariableB))
			return false;

		TVal ValueA = VariableA->GetValue();
		TVal ValueB = VariableB->GetValue();

		bool bResult = ValueA <= ValueB;

		UCadenceVariableBool* ResultVariable = Cast<UCadenceVariableBool>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(bResult);
		return true;
	}
}

TSubclassOf<UCadenceVariable> UCadenceOpComparison::GetResultType() const
{
	return UCadenceVariableBool::StaticClass();
}

bool UCadenceOpEqual::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	bool bResult = true;

	for(UCadenceVariable* VariableB : InVariableBs)
	{
		if(!InVariableA->Equals(VariableB))
		{
			bResult = false;
			break;
		}
	}
	
	UCadenceVariableBool* ResultBool = Cast<UCadenceVariableBool>(InResultVariable);
	if(!ensure(ResultBool))
		return false;

	ResultBool->SetValue(bResult);

	return true;
}

bool UCadenceOpNotEqual::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	bool bResult = true;

	for(UCadenceVariable* VariableB : InVariableBs)
	{
		if(InVariableA->Equals(VariableB))
		{
			bResult = false;
			break;
		}
	}
	
	UCadenceVariableBool* ResultBool = Cast<UCadenceVariableBool>(InResultVariable);
	if(!ensure(ResultBool))
		return false;

	ResultBool->SetValue(bResult);

	return true;
}

// Given we don't support conditional evaluation at this stage, this is essentially just a boolean only equal operation
// TODO: Reimplement this node when conditional evaluation is possible, if it ever is
bool UCadenceOpAND::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	bool bResult = true;

	for(UCadenceVariable* VariableB : InVariableBs)
	{
		if(!InVariableA->Equals(VariableB))
		{
			bResult = false;
			break;
		}
	}
	
	UCadenceVariableBool* ResultBool = Cast<UCadenceVariableBool>(InResultVariable);
	if(!ensure(ResultBool))
		return false;

	ResultBool->SetValue(bResult);

	return true;
}

TSubclassOf<UCadenceVariable> UCadenceOpAND::GetPrimaryType() const
{
	return UCadenceVariableBool::StaticClass();
}

bool UCadenceOpOR::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	bool bResult = false;

	for(UCadenceVariable* VariableB : InVariableBs)
	{
		bResult |= InVariableA->Equals(VariableB);		
	}
	
	UCadenceVariableBool* ResultBool = Cast<UCadenceVariableBool>(InResultVariable);
	if(!ensure(ResultBool))
		return false;

	ResultBool->SetValue(bResult);

	return true;
}

TSubclassOf<UCadenceVariable> UCadenceOpOR::GetPrimaryType() const
{
	return UCadenceVariableBool::StaticClass();
}

bool UCadenceOpGreaterThanInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOperation<UCadenceVariableInt, int32>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpGreaterThanFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOperation<UCadenceVariableFloat, float>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpGreaterThanDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOperation<UCadenceVariableDouble, double>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpGreaterThanOrEqualInt::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOrEqualOperation<UCadenceVariableInt, int32>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanOrEqualInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpGreaterThanOrEqualFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOrEqualOperation<UCadenceVariableFloat, float>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanOrEqualFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpGreaterThanOrEqualDouble::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformGreaterThanOrEqualOperation<UCadenceVariableDouble, double>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpGreaterThanOrEqualDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpLessThanInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOperation<UCadenceVariableInt, int32>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpLessThanFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOperation<UCadenceVariableFloat, float>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpLessThanDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOperation<UCadenceVariableDouble, double>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpLessThanOrEqualInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOrEqualOperation<UCadenceVariableInt, int32>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanOrEqualInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpLessThanOrEqualFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOrEqualOperation<UCadenceVariableFloat, float>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanOrEqualFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpLessThanOrEqualDouble::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	if(InVariableBs.Num() != 1)
		return false;
	
	return FCadenceComparisonOperationHelper::PerformLessThanOrEqualOperation<UCadenceVariableDouble, double>(InVariableA, InVariableBs[0], InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLessThanOrEqualDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}
