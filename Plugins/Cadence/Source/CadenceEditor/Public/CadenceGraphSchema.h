// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphDetailsTab.h"
#include "SPinTypeSelector.h"
#include "Graph/CadenceGraph.h"
#include "CadenceGraphOperationResolver.h"

#include "CadenceGraphSchema.generated.h"

class UCadenceGraphNodePin;
class UCadenceOperationNode_Base;
class UCadenceGraphEditor;
class UCadenceGraph;
class UCadenceAsset;
class UCadenceUserVariableGetterNode;
class UCadenceUserVariableSetterNode;
class UCadenceVariable;
/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	UCadenceGraphSchema();
	
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;	
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override;
	virtual void TrySetDefaultValue(UEdGraphPin& Pin, const FString& NewDefaultValue, bool bMarkAsModified = true) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual FLinearColor GetSecondaryPinTypeColor(const FEdGraphPinType& PinType) const override;

	virtual FText GetPinDisplayName(const UEdGraphPin* Pin) const override;

	virtual bool ConvertVariableToPinType(const UCadenceVariable* InVariable, FEdGraphPinType& OutPinType) const;

	virtual UCadenceUserVariableGetterNode* SpawnVariableGetNode(const FVector2D InGraphPosition, UCadenceGraph* InGraph, UCadenceVariable* InVariable) const;
	virtual UCadenceUserVariableSetterNode* SpawnVariableSetNode(const FVector2D InGraphPosition, UCadenceGraph* InGraph, UCadenceVariable* InVariable) const;
	bool IsExecPin(const UEdGraphPin& EdGraphPin) const;
	bool ArePinTypesCompatible(const FEdGraphPinType& InPinTypeA, const FEdGraphPinType& InPinTypeB) const;
	void GetVariableTypeTree(TArray<FPinTypeTreeItem>& OutTypeTreeArray, ETypeTreeFilter InTreeFilter) const;
	UClass* GetVariableClassFromPinType(const FEdGraphPinType& EdGraphPin) const;

	// Returns pointer to the variable or its replacement
	UCadenceVariable* ChangeVariableType(UCadenceVariable* InVar, UCadenceGraph* InGraph, UCadenceGraphEditor* InEditorGraph, const FEdGraphPinType& InEdGraphPinType) const;
	
	UCadenceVariable* AddNewUserVariable(TSubclassOf<UCadenceVariable> InClass, UCadenceGraph* InGraph) const;
	
	bool VariableAlreadyExistsWithName(TArray<FCadenceNamedVariable>& UserVariableArray, const FName& InName) const;
	FName GetUniqueDefaultVariableName(TArray<FCadenceNamedVariable>& UserVariableArray) const;

public:
	static const FName PC_Exec;
	static const FName PC_Wildcard;

	static bool IsVariablePinCategory(const FName& InPinCategory);
	
private:
	static const FString DefaultVariableNameBase;
	
	void GenerateColorMap();
	TArray<UCadenceVariable*> GetVariableTypeCDOs(bool InFilterForCreate = true) const;
	bool CheckOperationConnection(UCadenceOperationNode_Base* InOperationNode, UCadenceGraphNodePin* InPin, UClass* InOtherPinVarClass) const;
	void UpdateOperationNodeOperation(UCadenceOperationNode_Base* InNode) const;
	
	TMap<FName, FLinearColor> PinCategoryToColor;

	TSharedRef<FCadenceGraphOperationResolver> OperationResolver;
	static TSharedPtr<FCadenceGraphOperationResolver> OperationResolverStatic;
};
