// Copyright (C) Billy Fletcher 2025


#include "CadenceGraphEditor.h"

#include "CadenceGraphEditorNode.h"
#include "CadenceGraphPropertyCustomization.h"

void UCadenceGraphEditor::PostEditUndo()
{
	Super::PostEditUndo();
	RemoveDuplicateNodeEntries();
	CheckForUndoneDeletedNodes();
	OnUndoOrRedoPerformed.Broadcast();
}

void UCadenceGraphEditor::NotifyChildElementUndoOrRedo()
{
	OnUndoOrRedoPerformed.Broadcast();
}

void UCadenceGraphEditor::RemoveDuplicateNodeEntries()
{
	TArray<TObjectPtr<UEdGraphNode>> UniqueCopy;
	for(auto& Node : Nodes)
		UniqueCopy.AddUnique(Node);

	Modify();
	Nodes = UniqueCopy;
}

void UCadenceGraphEditor::CheckForUndoneDeletedNodes()
{
	for(UEdGraphNode* Node : Nodes)
	{
		if(UCadenceGraphEditorNode* CadenceEdNode = Cast<UCadenceGraphEditorNode>(Node))
		{
			UCadenceGraphNode* RuntimeNode = CadenceEdNode->GetRuntimeGraphNode();
			if(CadenceEdNode->Pins.IsEmpty() && IsValid(RuntimeNode))
				CadenceEdNode->Construct(RuntimeNode);
		}
	}
}
