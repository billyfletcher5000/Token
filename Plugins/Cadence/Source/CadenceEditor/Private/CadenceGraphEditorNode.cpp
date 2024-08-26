// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditorNode.h"

#include "Framework/Commands/GenericCommands.h"

void UCadenceGraphEditorNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaGeneral", FText::FromString(TEXT("General")));
	Section.AddMenuEntry(FGenericCommands::Get().Delete);
	Section.AddMenuEntry(FGenericCommands::Get().Cut);
	Section.AddMenuEntry(FGenericCommands::Get().Copy);
	Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
	Section.AddMenuEntry(FGenericCommands::Get().Paste);
}