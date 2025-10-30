// Copyright (C) Billy Fletcher 2025

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
	virtual void PostEditUndo() override;
	void NotifyChildElementUndoOrRedo();

	DECLARE_MULTICAST_DELEGATE(FOnUndoOrRedoPerformed);
	FOnUndoOrRedoPerformed OnUndoOrRedoPerformed;

private:
	void RemoveDuplicateNodeEntries();
	void CheckForUndoneDeletedNodes();
	
	UPROPERTY()
	TObjectPtr<UCadenceGraph> RuntimeGraph;
};
