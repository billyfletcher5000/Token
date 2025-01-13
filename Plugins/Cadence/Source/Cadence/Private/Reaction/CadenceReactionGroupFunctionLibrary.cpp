// Fill out your copyright notice in the Description page of Project Settings.


#include "Reaction/CadenceReactionGroupFunctionLibrary.h"

#include "Graph/CadenceVariable.h"
#include "Reaction/CadenceReactionGroup.h"

UCadenceVariable* UCadenceReactionGroupFunctionLibrary::GetReactionGroupVariable(UCadenceReactionGroup* InReactionGroup, FName VariableName)
{
	TArray<TObjectPtr<UCadenceVariable>> const& Variables = InReactionGroup->GetVariables();
	const TObjectPtr<UCadenceVariable>* Result = Variables.FindByPredicate([&VariableName] (UCadenceVariable* InVar)
	{
		return InVar->GetUserVariableName() == VariableName;
	});

	if(Result == nullptr)
		return nullptr;

	return *Result;
}
