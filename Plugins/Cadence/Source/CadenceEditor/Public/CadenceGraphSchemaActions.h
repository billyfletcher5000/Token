// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Graph/CadenceVariable.h"
#include "CadenceGraphSchemaActions.generated.h"

class UCadenceGraphNode;

USTRUCT()
struct FNewNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewNodeAction() {}
	FNewNodeAction(TSubclassOf<UCadenceGraphNode> InRuntimeNodeType, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), RuntimeNodeType(InRuntimeNodeType)
	{		
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;

protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location);
	
private:
	UPROPERTY()
	TSubclassOf<UCadenceGraphNode> RuntimeNodeType;
};

USTRUCT()
struct FNewVariableGetterNodeAction : public FNewNodeAction
{
	GENERATED_BODY()

public:
	FNewVariableGetterNodeAction() {}
	FNewVariableGetterNodeAction(FCadenceNamedVariable& InNamedVariable, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FNewNodeAction(nullptr, InNodeCategory, InMenuDesc, InToolTip, InGrouping), NamedVariable(InNamedVariable)
	{		
	}
	
protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location) override;
	
private:
	UPROPERTY()
	FCadenceNamedVariable NamedVariable;
};

USTRUCT()
struct FNewVariableSetterNodeAction : public FNewNodeAction
{
	GENERATED_BODY()

public:
	FNewVariableSetterNodeAction() {}
	FNewVariableSetterNodeAction(FCadenceNamedVariable& InNamedVariable, FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
	: FNewNodeAction(nullptr, InNodeCategory, InMenuDesc, InToolTip, InGrouping), NamedVariable(InNamedVariable)
	{		
	}
	
protected:
	virtual UCadenceGraphNode* CreateCadenceGraphNode(UCadenceGraph* RuntimeGraph, const FVector2D& Location) override;
	
private:
	UPROPERTY()
	FCadenceNamedVariable NamedVariable;
};

USTRUCT()
struct FCadenceVariableAction : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

private:
	TWeakObjectPtr<UCadenceVariable> Variable;

public:
	// Simple type info
	static FName StaticGetTypeId() {static FName Type("FCadenceVariableAction"); return Type;}
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 

	FCadenceVariableAction()
	: FEdGraphSchemaAction()
	{}

	FCadenceVariableAction(UCadenceVariable* InVariable)
	: FEdGraphSchemaAction(), Variable(InVariable)
	{}

	UCadenceVariable* GetVariable() const
	{
		return Variable.Get();
	}

	FName GetVariableName() const
	{
		return Variable->GetUserVariableName();
	}

	FString GetFriendlyVariableName() const
	{
		return Variable->GetUserVariableName().ToString();
	}

	virtual FEdGraphPinType GetPinType() const;

	virtual void ChangeVariableType(const FEdGraphPinType& NewPinType);

	virtual void RenameVariable(const FName& NewName) { Variable->SetUserVariableName(NewName); }

	virtual bool IsValidName(const FName& NewName, FText& OutErrorMessage) const;
	virtual void DeleteVariable();
	virtual bool IsVariableUsed();
	
	// FEdGraphSchemaAction interface
	virtual void MovePersistentItemToCategory(const FText& NewCategoryName) override;
	virtual int32 GetReorderIndexInContainer() const override;
	virtual bool ReorderToBeforeAction(TSharedRef<FEdGraphSchemaAction> OtherAction) override;
	virtual FEdGraphSchemaActionDefiningObject GetPersistentItemDefiningObject() const override;
	virtual bool IsAVariable() const { return true; }
	// End of FEdGraphSchemaAction interface
};