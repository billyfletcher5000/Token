// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphOperationResolver.h"

#include "Graph/Nodes/CadenceMathNodes.h"

namespace FCadenceGraphOpResolverHelper
{
	static FCadenceOperationBaseBucket* GetOrCreateBucket(FCadenceOperationBaseBucket& InRootBucket, const TSubclassOf<UCadenceOperation>& InClass)
	{
		if(InClass == InRootBucket.Class)
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

UE_DISABLE_OPTIMIZATION

TArray<FCadenceOperationResolverResult> FCadenceGraphOperationResolver::GetAppropriateOperationClasses(const TSubclassOf<UCadenceOperation>& InOperationBase,
	const TSubclassOf<UCadenceVariable>& InVariableClassA, const TSubclassOf<UCadenceVariable>& InVariableClassB,
	const TSubclassOf<UCadenceVariable>& InVariableClassReturn, const
	bool& InAreMultipleOfA, const bool& InAreMultipleOfB)
{
	// If all are 'wildcard' then there shouldn't be an operation as it's in its default state
	//if(InVariableClassA == nullptr && InVariableClassB == nullptr && InVariableClassReturn == nullptr)
	//	return {};
	
	if(!ensureMsgf((InAreMultipleOfA && InAreMultipleOfB) == false, TEXT("Operations can only support multiple pins for one of the two types!")))
		return {};
	
	TArray<FCadenceOperationResolverResult> Results;
	if(!InAreMultipleOfA)
	{
		 Results.Append(GetAppropriateOperationClassesInternal(
			InOperationBase, InVariableClassA, InVariableClassB, InVariableClassReturn, InAreMultipleOfB, false));		
	}

	Results.Append(GetAppropriateOperationClassesInternal(
		InOperationBase, InVariableClassB, InVariableClassA, InVariableClassReturn, InAreMultipleOfA, true));

	return Results;
}

TArray<FCadenceOperationResolverResult> FCadenceGraphOperationResolver::GetAppropriateOperationClassesInternal(
	const TSubclassOf<UCadenceOperation>& InOperationBase, const TSubclassOf<UCadenceVariable>& InVariableClassA,
	const TSubclassOf<UCadenceVariable>& InVariableClassB, const TSubclassOf<UCadenceVariable>& InVariableClassResult,
	const bool& InAreMultipleOfB, const bool& bIsReverseOrder)
{
	TArray<FCadenceOperationResolverResult> Result;
	FCadenceOperationBaseBucket* Bucket = FCadenceGraphOpResolverHelper::GetOrCreateBucket(RootBucket, InOperationBase);
	TArray<TSubclassOf<UCadenceOperation>> OperationTypes = FCadenceGraphOpResolverHelper::GatherClassesFromBucket(*Bucket);
	
	for(TSubclassOf<UCadenceOperation>& OperationType : OperationTypes)
	{
		if(!OperationType->IsChildOf(InOperationBase))
			continue;
		
		UCadenceOperation* OpCDO = OperationType->GetDefaultObject<UCadenceOperation>();
		
		if(!IsValid(OpCDO))
			continue;

		TSubclassOf<UCadenceVariable> ResultType = OpCDO->GetResultType();
		bool bResultTypeIsWildcard = InVariableClassResult == nullptr || ResultType == nullptr;
		bool bIsResultTypeSatisfied = bResultTypeIsWildcard || (ResultType == InVariableClassResult);
		if(!bIsResultTypeSatisfied)
			continue;

		TSubclassOf<UCadenceVariable> PrimaryType = OpCDO->GetPrimaryType();
		bool bPrimaryTypeIsWildcard = InVariableClassA == nullptr || PrimaryType == nullptr;
		bool bIsPrimaryTypeSatisfied = bPrimaryTypeIsWildcard || (PrimaryType == InVariableClassA);
		if(!bIsPrimaryTypeSatisfied)
			continue;

		if(!OpCDO->SupportsAdditionalSecondary() && InAreMultipleOfB)
			continue;

		TSubclassOf<UCadenceVariable> SecondaryType = OpCDO->GetSecondaryType();
		bool bSecondaryTypeIsWildcard = InVariableClassB == nullptr || SecondaryType == nullptr;
		bool bIsSecondaryTypeSatisfied = bSecondaryTypeIsWildcard || (SecondaryType == InVariableClassB);
		if(!bIsSecondaryTypeSatisfied)
			continue;
		
		Result.Add(FCadenceOperationResolverResult {OperationType, PrimaryType, SecondaryType, ResultType, bIsReverseOrder});		
	}

	return Result;
}

UE_ENABLE_OPTIMIZATION
