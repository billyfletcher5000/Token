// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceMathNodes.h"

#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

namespace FCadenceMathOperationHelper
{
	template<typename TVarA, typename TVarB, typename TVarR, typename TVal>
	bool ApplyAddOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVal OutValue = VariableA->GetValue();

		for(UCadenceVariable* VariableB : InVariableBs)
		{
			TVarB* VariableBCasted = Cast<TVarB>(VariableB);
			if(!ensure(VariableBCasted))
				return false;

			OutValue += VariableBCasted->GetValue();
		}

		TVarR* ResultVariable = Cast<TVarR>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(OutValue);
		return true;
	}

	template<typename TVarA, typename TVarB, typename TVarR, typename TVal>
	bool ApplySubtractOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVal OutValue = VariableA->GetValue();

		for(UCadenceVariable* VariableB : InVariableBs)
		{
			TVarB* VariableBCasted = Cast<TVarB>(VariableB);
			if(!ensure(VariableBCasted))
				return false;

			OutValue -= VariableBCasted->GetValue();
		}

		TVarR* ResultVariable = Cast<TVarR>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(OutValue);
		return true;
	}

	template<typename TVarA, typename TVarB, typename TVarR, typename TVal>
	bool ApplyMultiplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVal OutValue = VariableA->GetValue();

		for(UCadenceVariable* VariableB : InVariableBs)
		{
			TVarB* VariableBCasted = Cast<TVarB>(VariableB);
			if(!ensure(VariableBCasted))
				return false;

			OutValue *= VariableBCasted->GetValue();
		}

		TVarR* ResultVariable = Cast<TVarR>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(OutValue);
		return true;
	}

	template<typename TVarA, typename TVarB, typename TVarR, typename TVal>
	bool ApplyDivideOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
	{	
		TVarA* VariableA = Cast<TVarA>(InVariableA);
		if(!ensure(VariableA))
			return false;

		TVal OutValue = VariableA->GetValue();

		for(UCadenceVariable* VariableB : InVariableBs)
		{
			TVarB* VariableBCasted = Cast<TVarB>(VariableB);
			if(!ensure(VariableBCasted))
				return false;

			OutValue /= VariableBCasted->GetValue();
		}

		TVarR* ResultVariable = Cast<TVarR>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(OutValue);
		return true;
	}

	template<typename TVar, typename TVal>
	bool ApplyInterpolateOperation(UCadenceVariable* InVariableA, UCadenceVariable* InVariableB, const float& InT, UCadenceVariable* InResultVariable)
	{	
		TVar* VariableA = Cast<TVar>(InVariableA);
		if(!ensure(VariableA))
			return false;

		
		TVar* VariableB = Cast<TVar>(InVariableB);
		if(!ensure(VariableB))
			return false;

		TVal VarAValue = VariableA->GetValue();
		TVal VarBValue = VariableB->GetValue();

		TVal OutValue = VarAValue + ((VarBValue - VarAValue) * InT);

		TVar* ResultVariable = Cast<TVar>(InResultVariable);
		if(!ensure(ResultVariable))
			return false;

		ResultVariable->SetValue(OutValue);
		return true;
	}
}

bool UCadenceOpAddInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableInt, UCadenceVariableInt, UCadenceVariableInt, int32>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpAddFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableFloat, UCadenceVariableFloat, UCadenceVariableFloat, float>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpAddDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableDouble, UCadenceVariableDouble, UCadenceVariableDouble, double>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpAddVector::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableVector, UCadenceVariableVector, UCadenceVariableVector, FVector>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddVector::GetPrimaryType() const
{
	return UCadenceVariableVector::StaticClass();
}

bool UCadenceOpAddVector2D::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableVector2D, UCadenceVariableVector2D, UCadenceVariableVector2D, FVector2D>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddVector2D::GetPrimaryType() const
{
	return UCadenceVariableVector2D::StaticClass();
}

bool UCadenceOpAddRotator::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyAddOperation<UCadenceVariableRotator, UCadenceVariableRotator, UCadenceVariableRotator, FRotator>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpAddRotator::GetPrimaryType() const
{
	return UCadenceVariableRotator::StaticClass();
}

bool UCadenceOpSubtractInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableInt, UCadenceVariableInt, UCadenceVariableInt, int32>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpSubtractFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableFloat, UCadenceVariableFloat, UCadenceVariableFloat, float>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpSubtractDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableDouble, UCadenceVariableDouble, UCadenceVariableDouble, double>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpSubtractVector::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableVector, UCadenceVariableVector, UCadenceVariableVector, FVector>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractVector::GetPrimaryType() const
{
	return UCadenceVariableVector::StaticClass();
}

bool UCadenceOpSubtractVector2D::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableVector2D, UCadenceVariableVector2D, UCadenceVariableVector2D, FVector2D>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractVector2D::GetPrimaryType() const
{
	return UCadenceVariableVector2D::StaticClass();
}

bool UCadenceOpSubtractRotator::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplySubtractOperation<UCadenceVariableRotator, UCadenceVariableRotator, UCadenceVariableRotator, FRotator>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpSubtractRotator::GetPrimaryType() const
{
	return UCadenceVariableRotator::StaticClass();
}

bool UCadenceOpMultiplyInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableInt, UCadenceVariableInt, UCadenceVariableInt, int32>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpMultiplyFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableFloat, UCadenceVariableFloat, UCadenceVariableFloat, float>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpMultiplyDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableDouble, UCadenceVariableDouble, UCadenceVariableDouble, double>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpMultiplyVectorFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableVector, UCadenceVariableFloat, UCadenceVariableVector, FVector>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyVectorFloat::GetPrimaryType() const
{
	return UCadenceVariableVector::StaticClass();
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyVectorFloat::GetSecondaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpMultiplyVector2DFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableVector2D, UCadenceVariableFloat, UCadenceVariableVector2D, FVector2D>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyVector2DFloat::GetPrimaryType() const
{
	return UCadenceVariableVector2D::StaticClass();
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyVector2DFloat::GetSecondaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpMultiplyRotatorFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyMultiplyOperation<UCadenceVariableRotator, UCadenceVariableFloat, UCadenceVariableRotator, FRotator>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyRotatorFloat::GetPrimaryType() const
{
	return UCadenceVariableRotator::StaticClass();
}

TSubclassOf<UCadenceVariable> UCadenceOpMultiplyRotatorFloat::GetSecondaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpDivideInt::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyDivideOperation<UCadenceVariableInt, UCadenceVariableInt, UCadenceVariableInt, int32>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideInt::GetPrimaryType() const
{
	return UCadenceVariableInt::StaticClass();
}

bool UCadenceOpDivideFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyDivideOperation<UCadenceVariableFloat, UCadenceVariableFloat, UCadenceVariableFloat, float>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpDivideDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyDivideOperation<UCadenceVariableDouble, UCadenceVariableDouble, UCadenceVariableDouble, double>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpDivideVectorFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyDivideOperation<UCadenceVariableVector, UCadenceVariableFloat, UCadenceVariableVector, FVector>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideVectorFloat::GetPrimaryType() const
{
	return UCadenceVariableVector::StaticClass();
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideVectorFloat::GetSecondaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpDivideVector2DFloat::ApplyOperation(UCadenceVariable* InVariableA,
	TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyDivideOperation<UCadenceVariableVector2D, UCadenceVariableFloat, UCadenceVariableVector2D, FVector2D>(InVariableA, InVariableBs, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideVector2DFloat::GetPrimaryType() const
{
	return UCadenceVariableVector2D::StaticClass();
}

TSubclassOf<UCadenceVariable> UCadenceOpDivideVector2DFloat::GetSecondaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpLerpFloat::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyInterpolateOperation<UCadenceVariableFloat, float>(InVariableA, InVariableBs[0], T, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLerpFloat::GetPrimaryType() const
{
	return UCadenceVariableFloat::StaticClass();
}

bool UCadenceOpLerpDouble::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyInterpolateOperation<UCadenceVariableDouble, double>(InVariableA, InVariableBs[0], T, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLerpDouble::GetPrimaryType() const
{
	return UCadenceVariableDouble::StaticClass();
}

bool UCadenceOpLerpVector::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyInterpolateOperation<UCadenceVariableVector, FVector>(InVariableA, InVariableBs[0], T, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLerpVector::GetPrimaryType() const
{
	return UCadenceVariableVector::StaticClass();
}

bool UCadenceOpLerpVector2D::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyInterpolateOperation<UCadenceVariableVector2D, FVector2D>(InVariableA, InVariableBs[0], T, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLerpVector2D::GetPrimaryType() const
{
	return UCadenceVariableVector2D::StaticClass();
}

bool UCadenceOpLerpRotator::ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs,
	UCadenceVariable* InResultVariable)
{
	return FCadenceMathOperationHelper::ApplyInterpolateOperation<UCadenceVariableRotator, FRotator>(InVariableA, InVariableBs[0], T, InResultVariable);
}

TSubclassOf<UCadenceVariable> UCadenceOpLerpRotator::GetPrimaryType() const
{
	return UCadenceVariableRotator::StaticClass();
}

void UCadenceInterpolateNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariablePinDefault<UCadenceVariableFloat>(FCadencePinConstants::Pin_T, 0.0f, 2);
}

ECadenceNodeExecuteResult UCadenceInterpolateNode::Execute(UCadenceContext* InContext)
{
	float T = 0.0f;
	if(!GetInputPinValue<UCadenceVariableFloat>(FCadencePinConstants::Pin_T, T))
		return ECadenceNodeExecuteResult::Failed;

	UCadenceOpLerp* LerpOp = Cast<UCadenceOpLerp>(GetOperation());
	LerpOp->SetT(T);
	
	return Super::Execute(InContext);
}
