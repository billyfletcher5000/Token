// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceOperationNodes.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceMathNodes.generated.h"

namespace FCadenceMathNodeConstants
{	
	static const FText NodeCategory = FText::FromString(TEXT("Maths"));	
	static constexpr FLinearColor NodeTitleColor = FLinearColor(0.1f, 1.0f, 0.1f);
}

// Add Operations
UCLASS(Abstract)
class UCadenceOpAdd : public UCadenceOperation
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpAddInt : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpAddFloat : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpAddDouble : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpAddVector : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpAddVector2D : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpAddRotator : public UCadenceOpAdd
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Add Operations

// Subtract Operations
UCLASS(Abstract)
class UCadenceOpSubtract : public UCadenceOperation
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpSubtractInt : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpSubtractFloat : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpSubtractDouble : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpSubtractVector : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpSubtractVector2D : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpSubtractRotator : public UCadenceOpSubtract
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Subtract Operations

// Multiply Operations
UCLASS(Abstract)
class UCadenceOpMultiply : public UCadenceOperation
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpMultiplyInt : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpMultiplyFloat : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpMultiplyDouble : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpMultiplyVectorFloat : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpMultiplyVector2DFloat : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpMultiplyRotatorFloat : public UCadenceOpMultiply
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Multiply Operations

// Divide Operations
UCLASS(Abstract)
class UCadenceOpDivide : public UCadenceOperation
{
	GENERATED_BODY()
};

UCLASS()
class UCadenceOpDivideInt : public UCadenceOpDivide
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpDivideFloat : public UCadenceOpDivide
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpDivideDouble : public UCadenceOpDivide
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpDivideVectorFloat : public UCadenceOpDivide
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpDivideVector2DFloat : public UCadenceOpDivide
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Divide Operations

// Interpolation Operations
UCLASS(Abstract)
class UCadenceOpLerp : public UCadenceOperation
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FName GetPrimaryPinName() const override { return FCadencePinConstants::Pin_A; }
	virtual FName GetSecondaryPinName() const override  { return FCadencePinConstants::Pin_B; }
	virtual bool ShouldHideInputPinNames() const override { return false; }
#endif
		
	void SetT(const float& InT) { T = InT; }
	float GetT() const { return T; }

protected:
	float T;
};

UCLASS()
class UCadenceOpLerpFloat : public UCadenceOpLerp
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpLerpDouble : public UCadenceOpLerp
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpLerpVector : public UCadenceOpLerp
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpLerpVector2D : public UCadenceOpLerp
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};

UCLASS()
class UCadenceOpLerpRotator : public UCadenceOpLerp
{
	GENERATED_BODY()

public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* InResultVariable) override;

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const override;
	virtual bool SupportsAdditionalSecondary() const override { return true; }
#endif
};
// ~Interpolation Operations


// Nodes
UCLASS()
class CADENCE_API UCadenceAddNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpAdd::StaticClass(); } 
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Add")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("+")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceSubtractNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpSubtract::StaticClass(); }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Subtract")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("-")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceMultiplyNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpMultiply::StaticClass(); }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Multiply")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("*")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceDivideNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpDivide::StaticClass(); }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Divide")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("/")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceInterpolateNode : public UCadenceOperationNode_Base
{
	GENERATED_BODY()

public:
	void CreateInputPins() override;
	ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const override { return UCadenceOpLerp::StaticClass(); }
#endif
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Interpolate")); }
	virtual FText GetNodeShortDisplayName() const override { return FText::FromString(TEXT("Lerp")); }
	virtual FText GetNodeCategory() const override { return FCadenceMathNodeConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceMathNodeConstants::NodeTitleColor; }
};
// ~Nodes