// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphOperationResolver.h"

#include "Graph/Nodes/CadenceMathNodes.h"

namespace FCadenceGraphOpResolverHelper
{
	static FCadenceOperationBaseBucket* GetOrCreateBucket(FCadenceOperationBaseBucket& InRootBucket, const TSubclassOf<UCadenceOperation>& InClass)
	{
		if(InClass->GetSuperClass() == InRootBucket.Class)
		{
			return &InRootBucket;
		}

		TSubclassOf<UCadenceOperation> BucketClass = InClass;
		while(BucketClass->GetSuperClass() != InRootBucket.Class)
		{
			BucketClass = BucketClass->GetSuperClass();
		}

		FCadenceOperationBaseBucket* Result = InRootBucket.ChildBuckets.FindByPredicate(
			[&BucketClass](const FCadenceOperationBaseBucket& InBucket)
			{
				return InBucket.Class == BucketClass;
			});

		if(Result)
		{				
			return GetOrCreateBucket(*Result, InClass);
		}

		FCadenceOperationBaseBucket NewBucket;
		NewBucket.Class = BucketClass;
		InRootBucket.ChildBuckets.Add(NewBucket);
		return GetOrCreateBucket(InRootBucket.ChildBuckets.Last(), InClass);		
	}
	
	static void GatherClassesFromBucket(FCadenceOperationBaseBucket& InBucket, TArray<TSubclassOf<UCadenceOperation>>& OutClasses)
	{
		OutClasses.Append(InBucket.LeafClasses);
		for(auto& ChildBucket : InBucket.ChildBuckets)
		{
			GatherClassesFromBucket(ChildBucket, OutClasses);
		}
	}
	
	static TArray<TSubclassOf<UCadenceOperation>> GatherClassesFromBucket(FCadenceOperationBaseBucket& InBucket)
	{		
		TArray<TSubclassOf<UCadenceOperation>> Output;
		GatherClassesFromBucket(InBucket, Output);
		return Output;
	}
}

FCadenceGraphOperationResolver::FCadenceGraphOperationResolver()
{
	UpdateClassList();
}

void FCadenceGraphOperationResolver::UpdateClassList()
{
	RootBucket.Class = UCadenceOperation::StaticClass();
	RootBucket.ChildBuckets.Empty();
	RootBucket.LeafClasses.Empty();

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceOperation::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			FCadenceOperationBaseBucket* Bucket = FCadenceGraphOpResolverHelper::GetOrCreateBucket(RootBucket, Class);
			Bucket->LeafClasses.Add(Class);
		}
	}
}

UCadenceOperation* FCadenceGraphOperationResolver::TryCreateMostAppropriateOperation(const TSubclassOf<UCadenceOperation>& InOperationBase,
	const TSubclassOf<UCadenceVariable>& InVariableClassA, const TSubclassOf<UCadenceVariable>& InVariableClassB,
	const TSubclassOf<UCadenceVariable>& InVariableClassReturn, UObject* InOuter, bool& OutPrimarySecondaryReversed, const
	bool& InAreMultipleOfA, const bool& InAreMultipleOfB)
{
	// If all are 'wildcard' then there shouldn't be an operation as it's in its default state
	if(InVariableClassA == nullptr && InVariableClassB == nullptr && InVariableClassReturn == nullptr)
		return nullptr;
	
	if(!ensureMsgf((InAreMultipleOfA && InAreMultipleOfB) == false, TEXT("Operations can only support multiple pins for one of the two types!")))
		return nullptr;
	
	UCadenceOperation* Operation = nullptr;
	
	if(!InAreMultipleOfA)
	{
		Operation = TryCreateMostAppropriateOperation(InOperationBase, InVariableClassA, InVariableClassB, InVariableClassReturn, InOuter, InAreMultipleOfB);
		if(IsValid(Operation))
		{
			OutPrimarySecondaryReversed = false;
			return Operation;
		}
	}

	Operation = TryCreateMostAppropriateOperation(InOperationBase, InVariableClassB, InVariableClassA, InVariableClassReturn, InOuter, InAreMultipleOfA);
	if(IsValid(Operation))
	{
		OutPrimarySecondaryReversed = true;
		return Operation;
	}

	return nullptr;
}

UCadenceOperation* FCadenceGraphOperationResolver::TryCreateMostAppropriateOperation(
	const TSubclassOf<UCadenceOperation>& InOperationBase, const TSubclassOf<UCadenceVariable>& InVariableClassA,
	const TSubclassOf<UCadenceVariable>& InVariableClassB, const TSubclassOf<UCadenceVariable>& InVariableClassResult,
	UObject* InOuter, const bool& InAreMultipleOfB)
{
	FCadenceOperationBaseBucket* Bucket = FCadenceGraphOpResolverHelper::GetOrCreateBucket(RootBucket, InOperationBase);
	TArray<TSubclassOf<UCadenceOperation>> OperationTypes = FCadenceGraphOpResolverHelper::GatherClassesFromBucket(*Bucket);
	
	for(TSubclassOf<UCadenceOperation>& OperationType : OperationTypes)
	{
		if(!OperationType->IsChildOf(InOperationBase))
			continue;
		
		UCadenceOperation* OpCDO = OperationType->GetDefaultObject<UCadenceOperation>();
		
		if(!IsValid(OpCDO))
			continue;
		
		bool bResultTypeIsWildcard = InVariableClassResult == nullptr;
		bool bIsResultTypeSatisfied = bResultTypeIsWildcard || (OpCDO->GetResultType() == InVariableClassA);
		if(!bIsResultTypeSatisfied)
			continue;

		bool bPrimaryTypeIsWildcard = InVariableClassA == nullptr;
		bool bIsPrimaryTypeSatisfied = bPrimaryTypeIsWildcard || (OpCDO->GetPrimaryType() == InVariableClassA);
		if(!bIsPrimaryTypeSatisfied)
			continue;

		if(!OpCDO->SupportsAdditionalSecondary() && InAreMultipleOfB)
			continue;
		
		bool bSecondaryTypeIsWildcard = InVariableClassB == nullptr;
		bool bIsSecondaryTypeSatisfied = bSecondaryTypeIsWildcard || (OpCDO->GetPrimaryType() == InVariableClassB);
		if(!bIsSecondaryTypeSatisfied)
			continue;
		
		return NewObject<UCadenceOperation>(InOuter, OperationType);		
	}

	return nullptr;
}
