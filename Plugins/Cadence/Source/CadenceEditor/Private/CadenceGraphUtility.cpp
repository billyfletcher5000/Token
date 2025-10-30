// Copyright (C) Billy Fletcher 2025


#include "CadenceGraphUtility.h"

#include "EdGraph/EdGraph.h"
#include "CadenceGraphEditorNode.h"

UCadenceGraphEditorNode* UCadenceGraphUtility::GetGraphEditorNodeForRuntimeNode(UEdGraph* InEditorGraph, UCadenceGraphNode* InRuntimeNode)
{
	UCadenceGraphEditorNode* Result = nullptr;

	for(UEdGraphNode* EdGraphNode : InEditorGraph->Nodes)
	{
		UCadenceGraphEditorNode* CadenceGraphEditorNode = Cast<UCadenceGraphEditorNode>(EdGraphNode);
		if(CadenceGraphEditorNode->GetRuntimeGraphNode() == InRuntimeNode)
			return CadenceGraphEditorNode;
	}
	
	return Result;
}
