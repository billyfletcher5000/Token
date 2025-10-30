// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"

class UCadenceGraphNode;
class UCadenceGraphEditorNode;

class UCadenceGraphUtility
{
public:
	static UCadenceGraphEditorNode* GetGraphEditorNodeForRuntimeNode(UEdGraph* InEditorGraph, UCadenceGraphNode* InRuntimeNode);
};
