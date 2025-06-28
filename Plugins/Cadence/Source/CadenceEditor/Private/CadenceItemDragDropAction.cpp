// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceItemDragDropAction.h"

#include "CadenceAsset.h"
#include "CadenceGraphApplication.h"
#include "CadenceGraphEditorNode.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphSchemaActions.h"
#include "CoreTypes.h"
#include "EdGraph/EdGraphSchema.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceVariable.h"
#include "Graph/Nodes/CadenceUserVariableNodes.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Math/Color.h"
#include "Styling/AppStyle.h"
#include "UObject/NameTypes.h"

struct FSlateBrush;

#define LOCTEXT_NAMESPACE "FCadenceItemDragDropAction"

FCadenceItemDragDropAction::FCadenceItemDragDropAction()
	: bControlDrag(false)
	, bAltDrag(false)
{
}

FReply FCadenceItemDragDropAction::DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action)
{
	if (SourceAction.IsValid() && (SourceAction->GetTypeId() == Action->GetTypeId()))
	{
		if (SourceAction->GetPersistentItemDefiningObject() == Action->GetPersistentItemDefiningObject())
		{
			SourceAction->ReorderToBeforeAction(Action);
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply FCadenceItemDragDropAction::DroppedOnCategory(FText Category)
{
	if (SourceAction.IsValid())
	{
		SourceAction->MovePersistentItemToCategory(Category);
	}
	return FReply::Handled();
}

FReply FCadenceItemDragDropAction::DroppedOnPanel(const TSharedRef< SWidget >& Panel, const FVector2f& ScreenPosition, const FVector2f& GraphPosition, UEdGraph& Graph)
{
	FReply Reply = FReply::Unhandled();

	FText CannotDropReason = FText::GetEmpty();
	if (!CanBeDroppedDelegate.IsBound() || CanBeDroppedDelegate.Execute(SourceAction, GetHoveredGraph(), CannotDropReason))
	{
		Reply = FGraphSchemaActionDragDropAction::DroppedOnPanel(Panel, ScreenPosition, GraphPosition, Graph);
	}

	return Reply;
}

void FCadenceItemDragDropAction::HoverTargetChanged()
{
	UEdGraph* TheHoveredGraph = GetHoveredGraph();

	FText CannotDropReason = FText::GetEmpty();
	if (ActionWillShowExistingNode())
	{
		FSlateBrush const* ShowsExistingIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.ShowNode"));
		FText DragingText = FText::Format(LOCTEXT("ShowExistingNode", "Show '{0}'"), SourceAction->GetMenuDescription());
		SetSimpleFeedbackMessage(ShowsExistingIcon, FLinearColor::White, DragingText);
	}
	// it should be obvious that we can't drop on anything but a graph, so no need to point that out
	else if ((TheHoveredGraph == nullptr) || !CanBeDroppedDelegate.IsBound() || CanBeDroppedDelegate.Execute(SourceAction, TheHoveredGraph, CannotDropReason))
	{	
		if (SourceAction.IsValid())
		{
			if (!HoveredCategoryName.IsEmpty())
			{
				const bool bIsNative = !SourceAction->GetPersistentItemDefiningObject().IsPotentiallyEditable();

				FFormatNamedArguments Args;
				Args.Add(TEXT("DisplayName"), SourceAction->GetMenuDescription());
				Args.Add(TEXT("HoveredCategoryName"), HoveredCategoryName);

				if (bIsNative)
				{
					SetFeedbackMessageError(FText::Format(LOCTEXT("ChangingCatagoryNotEditable", "Cannot change category for '{DisplayName}' because it is declared in C++"), Args));
				}
				else if (HoveredCategoryName.EqualTo(SourceAction->GetCategory()))
				{
					SetFeedbackMessageError(FText::Format(LOCTEXT("ChangingCatagoryAlreadyIn", "'{DisplayName}' is already in category '{HoveredCategoryName}'"), Args));
				}
				else
				{
					SetFeedbackMessageOK(FText::Format(LOCTEXT("ChangingCatagoryOk", "Move '{DisplayName}' to category '{HoveredCategoryName}'"), Args));
				}
				return;
			}
			else if (HoveredAction.IsValid())
			{
				TSharedPtr<FEdGraphSchemaAction> HoveredActionPtr = HoveredAction.Pin();
				FFormatNamedArguments Args;
				Args.Add(TEXT("DraggedDisplayName"), SourceAction->GetMenuDescription());
				Args.Add(TEXT("DropTargetDisplayName"), HoveredActionPtr->GetMenuDescription());

				if (HoveredActionPtr->GetTypeId() == SourceAction->GetTypeId())
				{
					if (SourceAction->GetPersistentItemDefiningObject() == HoveredActionPtr->GetPersistentItemDefiningObject())
					{
						const int32 MovingItemIndex = SourceAction->GetReorderIndexInContainer();
						const int32 TargetVarIndex = HoveredActionPtr->GetReorderIndexInContainer();

						if (MovingItemIndex == INDEX_NONE)
						{
							SetFeedbackMessageError(FText::Format(LOCTEXT("ReorderNonOrderedItem", "Cannot reorder '{DraggedDisplayName}'."), Args));
						}
						else if (TargetVarIndex == INDEX_NONE)
						{
							SetFeedbackMessageError(FText::Format(LOCTEXT("ReorderOntoNonOrderedItem", "Cannot reorder '{DraggedDisplayName}' before '{DropTargetDisplayName}'."), Args));
						}
						else if (HoveredActionPtr == SourceAction)
						{
							SetFeedbackMessageError(FText::Format(LOCTEXT("ReorderOntoSameItem", "Cannot reorder '{DraggedDisplayName}' before itself."), Args));
						}
						else
						{
							SetFeedbackMessageOK(FText::Format(LOCTEXT("ReorderActionOK", "Reorder '{DraggedDisplayName}' before '{DropTargetDisplayName}'"), Args));
						}
					}
					else
					{
						SetFeedbackMessageError(FText::Format(LOCTEXT("ReorderActionDifferentScope", "Cannot reorder '{DraggedDisplayName}' into a different scope."), Args));
					}
				}
				else
				{
					SetFeedbackMessageError(FText::Format(LOCTEXT("ReorderActionDifferentAction", "Cannot reorder '{DraggedDisplayName}' into a different section."), Args));
				}

				return;
			}
		}
		else 
		{
			SetFeedbackMessageError(CannotDropReason);
		}
	}

	FGraphSchemaActionDragDropAction::HoverTargetChanged();
}

void FCadenceItemDragDropAction::SetFeedbackMessageError(const FText& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, Message);
}

void FCadenceItemDragDropAction::SetFeedbackMessageOK(const FText& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, Message);
}

bool FCadenceItemDragDropAction::ActionWillShowExistingNode() const
{
	bool bWillFocusOnExistingNode = false;
	return bWillFocusOnExistingNode;
}

class SWidget;
struct FSlateBrush;
struct FSlateColor;

FCadenceVariableItemDragDropAction::FCadenceVariableItemDragDropAction()
	: Variable(nullptr)
{
}

TSharedRef<FCadenceVariableItemDragDropAction> FCadenceVariableItemDragDropAction::New(TSharedPtr<FEdGraphSchemaAction> InAction, UCadenceVariable* InVariable, UCadenceAsset* InAsset, TSharedPtr<FCadenceGraphApplication> InApplication)
{
	TSharedRef<FCadenceVariableItemDragDropAction> Operation = MakeShareable(new FCadenceVariableItemDragDropAction);
	Operation->Variable = InVariable;
	Operation->Asset = InAsset;
	Operation->SourceAction = InAction;
	Operation->Application = InApplication.ToWeakPtr();
	Operation->Construct();
	return Operation;
}

UCadenceAsset* FCadenceVariableItemDragDropAction::GetSourceAsset() const
{
	return Asset.Get();
}

void FCadenceVariableItemDragDropAction::HoverTargetChanged()
{
	UCadenceVariable* Var = GetVariable();
	if (Var == nullptr)
	{
		return;
	}

	// Icon/text to draw on tooltip
	FText Message = LOCTEXT("InvalidDropTarget", "Invalid drop target!");

	UEdGraphPin* PinUnderCursor = GetHoveredPin();

	bool bCanMakeSetter = true;
	UEdGraph* TheHoveredGraph = GetHoveredGraph();
	if (TheHoveredGraph)
	{
		FNodeConstructionParams NewNodeParams;
		NewNodeParams.Variable = Var;
		NewNodeParams.Graph = Application.Pin()->GetWorkingGraph();		
		NewNodeParams.EdGraph = TheHoveredGraph;
		
		bCanMakeSetter = CanExecuteMakeSetter(NewNodeParams, Var);
	}

	if (PinUnderCursor)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PinUnderCursor"), FText::FromName(PinUnderCursor->PinName));
		Args.Add(TEXT("VariableName"), FText::FromName(Variable->GetUserVariableName()));

		if (CanVariableBeDropped(Var, *PinUnderCursor->GetOwningNode()->GetGraph()))
		{
			if (PinUnderCursor->bOrphanedPin)
			{
				SetFeedbackMessageError(FText::Format(LOCTEXT("OrphanedPin_Error", "Cannot make connection to orphaned pin {PinUnderCursor}"), Args));
			}
			else if (const UCadenceGraphSchema* Schema = Cast<const UCadenceGraphSchema>(PinUnderCursor->GetSchema()))
			{
				const bool bIsExecPin = Schema->IsExecPin(*PinUnderCursor);

				const bool bIsRead = (PinUnderCursor->Direction == EGPD_Input) && !bIsExecPin;				
				const bool bWritableProperty = true; // TODO: Replace if we end up supporting read only variables
				const bool bCanWriteIfNeeded = bIsRead || bWritableProperty;

				FEdGraphPinType VariablePinType;
				Schema->ConvertVariableToPinType(Var, VariablePinType);
				const bool bTypeMatch = Schema->ArePinTypesCompatible(VariablePinType, PinUnderCursor->PinType) || bIsExecPin;
				
				Args.Add(TEXT("PinUnderCursor"), FText::FromName(PinUnderCursor->PinName));

				if (bTypeMatch && bCanWriteIfNeeded)
				{
					SetFeedbackMessageOK(bIsRead ?
						FText::Format(LOCTEXT("MakeThisEqualThat_PinEqualVariableName", "Make {PinUnderCursor} = {VariableName}"), Args) :
						FText::Format(LOCTEXT("MakeThisEqualThat_VariableNameEqualPin", "Make {VariableName} = {PinUnderCursor}"), Args));
				}
				else
				{
					SetFeedbackMessageError(bCanWriteIfNeeded ?
						FText::Format(LOCTEXT("NotCompatible_Error", "The type of '{VariableName}' is not compatible with {PinUnderCursor}"), Args) :
						FText::Format(LOCTEXT("ReadOnlyVar_Error", "Cannot write to read-only variable '{VariableName}'"), Args));
				}
			}
		}
		else
		{
			Args.Add(TEXT("Scope"), FText::FromString(PinUnderCursor->GetOwningNode()->GetGraph()->GetName()));

			SetFeedbackMessageError(FText::Format( LOCTEXT("IncorrectGraphForPin_Error", "Cannot place local variable '{VariableName}' in external scope '{Scope}'"), Args));
		}
	}
	
	if (bAltDrag && !bCanMakeSetter)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("VariableName"), FText::FromName(Variable->GetUserVariableName()));

		SetFeedbackMessageError(FText::Format(LOCTEXT("CannotPlaceSetter", "Variable '{VariableName}' is readonly, you cannot set this variable."), Args));
	}
	else
	{
		FCadenceItemDragDropAction::HoverTargetChanged();
	}
}

void FCadenceVariableItemDragDropAction::GetDefaultStatusSymbol(const FSlateBrush*& PrimaryBrushOut, FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut) const
{
	PrimaryBrushOut = FCadenceGraphApplication::GetVarIconAndColorFromVariable(Variable.Get(), IconColorOut, SecondaryBrushOut, SecondaryColorOut);
}

FReply FCadenceVariableItemDragDropAction::DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	if (UEdGraphPin* TargetPin = GetHoveredPin())
	{
		if (!TargetPin->bOrphanedPin)
		{
			if (const UCadenceGraphSchema* Schema = Cast<UCadenceGraphSchema>(TargetPin->GetSchema()))
			{
				const bool bIsExecPin = Schema->IsExecPin(*TargetPin);

				if (CanVariableBeDropped(Variable.Get(), *TargetPin->GetOwningNode()->GetGraph()))
				{
					const bool bIsRead = (TargetPin->Direction == EGPD_Input) && !bIsExecPin;

					FEdGraphPinType VariablePinType;
					Schema->ConvertVariableToPinType(Variable.Get(), VariablePinType);
					const bool bTypeMatch = Schema->ArePinTypesCompatible(VariablePinType, TargetPin->PinType) || bIsExecPin;

					UCadenceGraphEditorNode* EditorNode = Cast<UCadenceGraphEditorNode>(TargetPin->GetOwningNode());
					if(!IsValid(EditorNode))
						return FReply::Unhandled();

					UCadenceGraphNode* RuntimeNode = EditorNode->GetRuntimeGraphNode();
					if(!IsValid(RuntimeNode))
						return FReply::Unhandled();

					
					
					if (bTypeMatch)
					{
						if(bIsRead)
						{
							UCadenceUserVariableGetterNode* Node = FNewVariableGetterNodeAction::CreateGetterNode(Application.Pin()->GetWorkingGraph(), Variable.Get(), ScreenPosition);
							UCadenceGraphNodePin* RuntimePin = RuntimeNode->GetOutputPin(TargetPin->PinName);
							UCadenceGraphNodePin* GetterPin = Node->GetVariableOutputPin();

							if(!IsValid(RuntimeNode) || !IsValid(GetterPin))
								return FReply::Unhandled();
							
							GetterPin->ConnectPin(RuntimePin);
							return FReply::Handled();
						}
						else
						{
							UCadenceUserVariableSetterNode* Node = FNewVariableSetterNodeAction::CreateSetterNode(Application.Pin()->GetWorkingGraph(), Variable.Get(), ScreenPosition);

							UCadenceGraphNodePin* RuntimePin = RuntimeNode->GetOutputPin(TargetPin->PinName);
							UCadenceGraphNodePin* SetterPin = Node->GetVariableInputPin();

							if(!IsValid(RuntimeNode) || !IsValid(SetterPin))
								return FReply::Unhandled();
							
							RuntimePin->ConnectPin(SetterPin);
							return FReply::Handled();
						}
					}
				}
			}
		}
	}

	return FReply::Handled();
}

FReply FCadenceVariableItemDragDropAction::DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	return FReply::Unhandled();
}

void FCadenceVariableItemDragDropAction::MakeGetter(FNodeConstructionParams InParams)
{
	check(InParams.Graph);

	const UCadenceGraphSchema* CadenceGraphSchema = Cast<const UCadenceGraphSchema>(InParams.EdGraph->GetSchema());
	if (CadenceGraphSchema)
	{
		CadenceGraphSchema->SpawnVariableGetNode(InParams.GraphPosition, InParams.Graph, InParams.Variable);
	}
}

void FCadenceVariableItemDragDropAction::MakeSetter(FNodeConstructionParams InParams)
{
	check(InParams.Graph);

	const UCadenceGraphSchema* CadenceGraphSchema = Cast<const UCadenceGraphSchema>(InParams.EdGraph->GetSchema());
	if (CadenceGraphSchema)
	{
		CadenceGraphSchema->SpawnVariableSetNode(InParams.GraphPosition, InParams.Graph, InParams.Variable);
	}
}

bool FCadenceVariableItemDragDropAction::CanExecuteMakeSetter(FNodeConstructionParams InParams, UCadenceVariable* InVariable)
{
	return true;
}

FReply FCadenceVariableItemDragDropAction::DroppedOnPanel( const TSharedRef< SWidget >& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph)
{
	if (Graph.GetSchema()->IsA<UCadenceGraphSchema>())
	{
		if (Variable.IsValid() && CanVariableBeDropped(Variable.Get(), Graph))
		{			
			FNodeConstructionParams NewNodeParams;
			NewNodeParams.Variable = Variable.Get();
			NewNodeParams.Graph = Application.Pin()->GetWorkingGraph();
			NewNodeParams.EdGraph = &Graph;
			NewNodeParams.GraphPosition = GraphPosition;

			// Take into account current state of modifier keys in case the user changed their mind
			FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
			const bool bModifiedKeysActive = ModifierKeys.IsControlDown() || ModifierKeys.IsAltDown();
			const bool bAutoCreateGetter = bModifiedKeysActive ? ModifierKeys.IsControlDown() : bControlDrag;
			const bool bAutoCreateSetter = bModifiedKeysActive ? ModifierKeys.IsAltDown() : bAltDrag;
			// Handle Getter/Setters
			if (bAutoCreateGetter || bAutoCreateSetter)
			{
				if (bAutoCreateGetter || !CanExecuteMakeSetter(NewNodeParams, Variable.Get()))
				{
					MakeGetter(NewNodeParams);
					NewNodeParams.GraphPosition.Y += 50.f;
				}
				if (bAutoCreateSetter && CanExecuteMakeSetter( NewNodeParams, Variable.Get()))
				{
					MakeSetter(NewNodeParams);
				}
			}
			// Show selection menu
			else
			{
				FMenuBuilder MenuBuilder(true, NULL);
				const FText VariableNameText = FText::FromName( Variable->GetUserVariableName() );

				MenuBuilder.BeginSection("BPVariableDroppedOn", VariableNameText );

				MenuBuilder.AddMenuEntry(
					FText::Format( LOCTEXT("CreateGetVariable", "Get {0}"), VariableNameText ),
					FText::Format( LOCTEXT("CreateVariableGetterToolTip", "Create Getter for variable '{0}'\n(Ctrl-drag to automatically create a getter)"), VariableNameText ),
					FSlateIcon(),
					FUIAction(
					FExecuteAction::CreateStatic(&FCadenceVariableItemDragDropAction::MakeGetter, NewNodeParams), FCanExecuteAction())
					);

				MenuBuilder.AddMenuEntry(
					FText::Format( LOCTEXT("CreateSetVariable", "Set {0}"), VariableNameText ),
					FText::Format( LOCTEXT("CreateVariableSetterToolTip", "Create Setter for variable '{0}'\n(Alt-drag to automatically create a setter)"), VariableNameText ),
					FSlateIcon(),
					FUIAction(
					FExecuteAction::CreateStatic(&FCadenceVariableItemDragDropAction::MakeSetter, NewNodeParams),
					FCanExecuteAction::CreateStatic(&FCadenceVariableItemDragDropAction::CanExecuteMakeSetter, NewNodeParams, Variable.Get() ))
					);

				TSharedRef< SWidget > PanelWidget = Panel;
				// Show dialog to choose getter vs setter
				FSlateApplication::Get().PushMenu(
					PanelWidget,
					FWidgetPath(),
					MenuBuilder.MakeWidget(),
					ScreenPosition,
					FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu)
					);

				MenuBuilder.EndSection();
			}
		}
	}

	return FReply::Handled();
}

bool FCadenceVariableItemDragDropAction::CanVariableBeDropped(const UCadenceVariable* InVariable, const UEdGraph& InGraph) const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
