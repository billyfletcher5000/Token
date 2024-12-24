// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditor.h"

void UCadenceGraphEditor::PreEditUndo()
{
	Super::PreEditUndo();
	OnUndoOrRedoPerformed.Broadcast(this);
}