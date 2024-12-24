// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "CadenceGraphEditor.generated.h"

class UCadenceGraph;

/**
 * 
 */
UCLASS()
class CADENCEEDITOR_API UCadenceGraphEditor : public UEdGraph
{
	GENERATED_BODY()

public:
	TObjectPtr<UCadenceGraph> GetRuntimeGraph() const { return RuntimeGraph; }
	void SetRuntimeGraph(TObjectPtr<UCadenceGraph> InGraph) { RuntimeGraph = InGraph; }
	virtual void PreEditUndo() override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUndoOrRedoPerformed, UCadenceGraphEditor*);
	FOnUndoOrRedoPerformed OnUndoOrRedoPerformed;

private:
	UPROPERTY()
	TObjectPtr<UCadenceGraph> RuntimeGraph;
};
