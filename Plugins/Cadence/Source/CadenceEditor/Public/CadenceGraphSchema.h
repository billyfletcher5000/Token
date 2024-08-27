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

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;

public:
	static const FName PC_Variable;
	static const FName PC_Exec;

private:
	void GenerateColorMap();
	
private:
	TMap<FName, FLinearColor> SubCategoryToColor;
};
