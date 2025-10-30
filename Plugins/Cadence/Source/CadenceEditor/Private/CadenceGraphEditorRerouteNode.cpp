// Copyright (C) Billy Fletcher 2025


#include "CadenceGraphEditorRerouteNode.h"

#include "CadenceGraphSchema.h"
#include "SGraphNodeKnot.h"

void UCadenceGraphEditorRerouteNode::AllocateDefaultPins()
{
}

FText UCadenceGraphEditorRerouteNode::GetTooltipText() const
{
	return FText::FromString(TEXT("Reroute Node (reroutes connections)"));
}

FText UCadenceGraphEditorRerouteNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::EditableTitle)
	{
		return FText::FromString(NodeComment);
	}
	else if (TitleType == ENodeTitleType::MenuTitle)
	{
		return FText::FromString(TEXT("Add Reroute Node..."));
	}
	else
	{
		return FText::FromString(TEXT("Reroute Node"));
	}
}

bool UCadenceGraphEditorRerouteNode::ShouldOverridePinNames() const
{
	return true;
}

FText UCadenceGraphEditorRerouteNode::GetPinNameOverride(const UEdGraphPin& Pin) const
{
	return FText::GetEmpty();
}

void UCadenceGraphEditorRerouteNode::OnRenameNode(const FString& NewName)
{
	NodeComment = NewName;
}

TSharedPtr<INameValidatorInterface> UCadenceGraphEditorRerouteNode::MakeNameValidator() const
{
	return MakeShareable(new FDummyNameValidator(EValidatorResult::Ok));
}

bool UCadenceGraphEditorRerouteNode::CanSplitPin(const UEdGraphPin* Pin) const
{
	return false;
}

UEdGraphPin* UCadenceGraphEditorRerouteNode::GetPassThroughPin(const UEdGraphPin* FromPin) const
{
	if(FromPin && Pins.Contains(FromPin))
	{
		return FromPin == Pins[0] ? Pins[1] : Pins[0];
	}

	return nullptr;
}

TSharedPtr<SGraphNode> UCadenceGraphEditorRerouteNode::CreateVisualWidget()
{
	return SNew(SGraphNodeKnot, this);
}