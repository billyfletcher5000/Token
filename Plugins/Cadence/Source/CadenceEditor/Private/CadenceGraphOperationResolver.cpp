// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphOperationResolver.h"

#include "Graph/Nodes/CadenceMathNodes.h"

FCadenceGraphOperationResolver::FCadenceGraphOperationResolver()
{
	UpdateClassList();
}

void FCadenceGraphOperationResolver::UpdateClassList()
{
	OperationTypes.Empty();

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceOperation::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			OperationTypes.Add(Class);
		}
	}
}

UCadenceOperation* FCadenceGraphOperationResolver::TryCreateMostAppropriateOperation(const TSubclassOf<UCadenceOperation>& InOperationBase, 
	const TSubclassOf<UCadenceVariable>& InVariableClassA, const TSubclassOf<UCadenceVariable>& InVariableClassB,
	UObject* InOuter, const bool& InAreMultipleOfA, const bool& InAreMultipleOfB)
{
	ensureMsgf((InAreMultipleOfA && InAreMultipleOfB) == false, TEXT("Operations can only support multiple pins for one of the two types!"));	
	
	for(TSubclassOf<UCadenceOperation>& OperationType : OperationTypes)
	{
		if(!OperationType->IsChildOf(InOperationBase))
			continue;
		
		UCadenceOperation* OpCDO = OperationType->GetDefaultObject<UCadenceOperation>();
		
		if(!IsValid(OpCDO))
			continue;

		if(OpCDO->IsMatchForClass(InVariableClassA, InVariableClassB)
			&& (!InAreMultipleOfA && (!InAreMultipleOfB || OpCDO->SupportsAdditionalSecondary())))
		{
			return NewObject<UCadenceOperation>(InOuter, OperationType);
		}
		
		if(OpCDO->IsMatchForClass(InVariableClassB, InVariableClassA)
			&& (!InAreMultipleOfB && (!InAreMultipleOfA || OpCDO->SupportsAdditionalSecondary())))
		{
			return NewObject<UCadenceOperation>(InOuter, OperationType);
		}
	}

	return nullptr;
}
