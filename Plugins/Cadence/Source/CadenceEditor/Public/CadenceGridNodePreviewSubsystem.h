// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "CadenceGridNodePreviewSubsystem.generated.h"

class UCadenceGraphGridPreviewCommandDecorator;
/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceGridNodePreviewSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UCadenceGraphGridPreviewCommandDecorator* GetDecoratorForCommandType(UScriptStruct* InCommandType) const;

private:
	UPROPERTY()
	TMap<TObjectPtr<UScriptStruct>, TObjectPtr<UCadenceGraphGridPreviewCommandDecorator>> CommandTypeToDecorator;
};
