// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UCadenceGraphNode;
class UCadenceGraphEditorNode;

class UCadenceGraphUtility
{
public:
	static UCadenceGraphEditorNode* GetGraphEditorNodeForRuntimeNode(UEdGraph* InEditorGraph, UCadenceGraphNode* InRuntimeNode);
};
