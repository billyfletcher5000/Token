// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CadenceGraphSchema.generated.h"

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

	virtual FText GetPinDisplayName(const UEdGraphPin* Pin) const override;

public:
	static const FName PC_Variable;
	static const FName PC_Int;
	static const FName PC_Exec;
	static const FName PC_Wildcard;

	static bool IsVariablePinCategory(const FName& InPinCategory);
	
private:
	void GenerateColorMap();
	
private:
	TMap<FName, FLinearColor> PinCategoryToColor;
};
