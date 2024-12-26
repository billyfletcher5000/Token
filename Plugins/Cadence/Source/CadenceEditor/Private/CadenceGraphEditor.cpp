// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditor.h"

void UCadenceGraphEditor::PostEditUndo()
{
	Super::PostEditUndo();
	OnUndoOrRedoPerformed.Broadcast();
}

void UCadenceGraphEditor::NotifyChildElementUndoOrRedo()
{
	OnUndoOrRedoPerformed.Broadcast();
}
