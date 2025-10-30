// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceOperationNodes.h"
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

// Comparison Base
UCLASS(Abstract)
class UCadenceOpComparison : public UCadenceOperation
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetResultType() const override;
#endif
};
// ~Comparison Base

// Equal Operations
UCLASS()
class UCadenceOpEqual : public UCadenceOpComparison
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Equal Operations

// Not Equal Operations
UCLASS()
class UCadenceOpNotEqual : public UCadenceOpComparison
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Not Equal Operations

// Equal Operations
UCLASS()
class UCadenceOpAND : public UCadenceOperation
{
	GENERATED_BODY()
	
public:
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Equal Operations

// Equal Operations
UCLASS()
class UCadenceOpOR : public UCadenceOperation
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Equal Operations

// Greater Than Operations
UCLASS(Abstract)
class UCadenceOpGreaterThan : public UCadenceOpComparison
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpGreaterThanInt : public UCadenceOpGreaterThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpGreaterThanFloat : public UCadenceOpGreaterThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpGreaterThanDouble : public UCadenceOpGreaterThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};
// ~Greater Than Operations

// Greater Than Or Equal Operations
UCLASS(Abstract)
class UCadenceOpGreaterThanOrEqual : public UCadenceOpComparison
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpGreaterThanOrEqualInt : public UCadenceOpGreaterThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpGreaterThanOrEqualFloat : public UCadenceOpGreaterThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpGreaterThanOrEqualDouble : public UCadenceOpGreaterThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};
// ~Greater Than Or Equal Operations

// Less Than Operations
UCLASS(Abstract)
class UCadenceOpLessThan : public UCadenceOpComparison
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpLessThanInt : public UCadenceOpLessThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpLessThanFloat : public UCadenceOpLessThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpLessThanDouble : public UCadenceOpGreaterThan
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};
// ~Less Than Operations

// Less Than Or Equal Operations
UCLASS(Abstract)
class UCadenceOpLessThanOrEqual : public UCadenceOpComparison
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpLessThanOrEqualInt : public UCadenceOpLessThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpLessThanOrEqualFloat : public UCadenceOpLessThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};

UCLASS()
class UCadenceOpLessThanOrEqualDouble : public UCadenceOpLessThanOrEqual
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
#endif
};
// ~Less Than Or Equal Operations

// Nodes
UCLASS()
class CADENCE_API UCadenceEqualNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpEqual::StaticClass(); } 
	virtual bool SecondarySharesPrimaryWildcard() const override { return true; }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Equal")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("==")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceNotEqualNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpNotEqual::StaticClass(); } 
	virtual bool SecondarySharesPrimaryWildcard() const override { return true; }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Not Equal")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("!=")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceANDNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpAND::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("AND")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("&&")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceORNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpOR::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("OR")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("||")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceGreaterThanNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpGreaterThan::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Greater Than")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT(">")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceGreaterThanOrEqualNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpGreaterThanOrEqual::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Greater Than Or Equal")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT(">=")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceLessThanNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpLessThan::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Less Than")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("<")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceLessThanOrEqualNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpLessThanOrEqual::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Less Than Or Equal")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("<=")); }
	virtual FText GetNodeCategory() const override { return FCadenceComparisonNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceComparisonNodeConstants::NodeTitleColor; }
};
// ~Nodes

