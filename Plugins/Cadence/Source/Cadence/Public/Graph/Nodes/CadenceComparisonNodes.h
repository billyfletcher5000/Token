// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"

#include "CadenceComparisonNodes.generated.h"

class UCadenceVariable;


namespace FCadenceComparisonNodeConstants
{	
	static const FText NodeCategory = FText::FromString(TEXT("Comparison"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.5f, 0.5f, 0.5f);

	static const FName Pin_A = TEXT("A");
	static const FName Pin_B = TEXT("B");
	static const FName Pin_Result = TEXT("Result");
}

UCLASS(Abstract)
class CADENCE_API UCadenceComparisonNode_Base : public UCadenceGraphNode
{
	GENERATED_BODY()
public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	virtual bool IsPure() const override { return true; }
	
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }

protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const PURE_VIRTUAL(UCadenceComparisonNode_Base::GetAllowedWildcardTypes, { return {}; });
	virtual bool Compare(const int32& InValueA, const int32& InValueB) const PURE_VIRTUAL(UCadenceComparisonNode_Base::Compare, { return false; });
	virtual bool Compare(const float& InValueA, const float& InValueB) const PURE_VIRTUAL(UCadenceComparisonNode_Base::Compare, { return false; });
	virtual bool Compare(const double& InValueA, const double& InValueB) const PURE_VIRTUAL(UCadenceComparisonNode_Base::Compare, { return false; });

private:
	template<typename TVar>
	bool ProcessComparison(UCadenceGraphNodePin* InPinA, UCadenceGraphNodePin* InPinB, bool& OutResult);
};

UCLASS()
class CADENCE_API UCadenceComparisonEqualsNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("==")); }
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
};

UCLASS()
class CADENCE_API UCadenceComparisonNotEqualsNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("!=")); }
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
};

UCLASS()
class CADENCE_API UCadenceComparisonGreaterThanNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT(">")); }
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
	virtual bool Compare(const int32& InValueA, const int32& InValueB) const override { return InValueA > InValueB; }
	virtual bool Compare(const float& InValueA, const float& InValueB) const override { return InValueA > InValueB; }
	virtual bool Compare(const double& InValueA, const double& InValueB) const override { return InValueA > InValueB; }
};

UCLASS()
class CADENCE_API UCadenceComparisonGreaterThanOrEqualsNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT(">=")); }
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
	virtual bool Compare(const int32& InValueA, const int32& InValueB) const override { return InValueA >= InValueB; }
	virtual bool Compare(const float& InValueA, const float& InValueB) const override { return InValueA >= InValueB; }
	virtual bool Compare(const double& InValueA, const double& InValueB) const override { return InValueA >= InValueB; }
};

UCLASS()
class CADENCE_API UCadenceComparisonLessThanNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("<")); }
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
	virtual bool Compare(const int32& InValueA, const int32& InValueB) const override { return InValueA < InValueB; }
	virtual bool Compare(const float& InValueA, const float& InValueB) const override { return InValueA < InValueB; }
	virtual bool Compare(const double& InValueA, const double& InValueB) const override { return InValueA < InValueB; }
};

UCLASS()
class CADENCE_API UCadenceComparisonLessThanOrEqualsNode : public UCadenceComparisonNode_Base
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("<=")); }
	
protected:
	virtual TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const override;
	virtual bool Compare(const int32& InValueA, const int32& InValueB) const override { return InValueA <= InValueB; }
	virtual bool Compare(const float& InValueA, const float& InValueB) const override { return InValueA <= InValueB; }
	virtual bool Compare(const double& InValueA, const double& InValueB) const override { return InValueA <= InValueB; }
};



template <typename TVar>
bool UCadenceComparisonNode_Base::ProcessComparison(UCadenceGraphNodePin* InPinA, UCadenceGraphNodePin* InPinB,	bool& OutResult)
{
	TVar* VariableA = InPinA->GetVariable<TVar>();

	if(!VariableA)
		return false;

	TVar* VariableB  = InPinB->GetVariable<TVar>();

	if(!VariableB)
		return false;

	OutResult = Compare(VariableA->GetValue(), VariableB->GetValue());
	return true;	
}