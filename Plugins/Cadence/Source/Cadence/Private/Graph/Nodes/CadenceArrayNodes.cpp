// Copyright (C) Billy Fletcher 2025


#include "Graph/Nodes/CadenceArrayNodes.h"

#include "Cadence.h"

void UCadenceForEachNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
}

void UCadenceForEachNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputExecPin(FCadencePinConstants::Pin_LoopBody);
	AddOutputVariableWildcardPin(FCadencePinConstants::Pin_ArrayElement);
	AddOutputVariablePin(FCadencePinConstants::Pin_Index, UCadenceVariableInt::StaticClass());
	AddOutputExecPin(FCadencePinConstants::Pin_Completed);
}

ECadenceNodeExecuteResult UCadenceForEachNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArray = InputArrayPin->GetVariable<UCadenceVariableArray>();

	int32 Index = 0;
	if(PathwayToIndex.Contains(InContext->Pathway))
		Index = PathwayToIndex[InContext->Pathway];
	
	if(InputArray->GetSize() <= Index)
	{
		PathwayToIndex.Remove(InContext->Pathway);
		CurrentActuatingOutputExecPin = GetOutputPin(FCadencePinConstants::Pin_Completed);
		return ECadenceNodeExecuteResult::Complete;
	}

	UCadenceGraphNodePin* IndexOutputPin = GetOutputPin(FCadencePinConstants::Pin_Index);
	UCadenceVariableInt* IndexOutputVariable = IndexOutputPin->GetVariable<UCadenceVariableInt>();
	IndexOutputVariable->SetValue(Index);

	UCadenceGraphNodePin* ArrayElementOutputPin = GetOutputPin(FCadencePinConstants::Pin_ArrayElement);
	UCadenceVariable* ArrayElementOutputVariable = ArrayElementOutputPin->GetVariable();

	UCadenceVariable* InputArrayElementVariable = InputArray->GetElement(Index);

	ArrayElementOutputVariable->CopyValueFrom(InputArrayElementVariable);
	CurrentActuatingOutputExecPin = GetOutputPin(FCadencePinConstants::Pin_LoopBody);
	
	Index++;

	if(PathwayToIndex.Contains(InContext->Pathway))
		PathwayToIndex[InContext->Pathway] = Index;
	else
		PathwayToIndex.Add(InContext->Pathway, Index);
	
	return ECadenceNodeExecuteResult::AdditionalExecPinActuated;
}

TArray<UCadenceGraphNodePin*> UCadenceForEachNode::GetActuatingOutputExecPins() const
{
	return { CurrentActuatingOutputExecPin.Get() };
}

void UCadenceArraySizeNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
}

void UCadenceArraySizeNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Size, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArraySizeNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* OutputSizePin = GetOutputPin(FCadencePinConstants::Pin_Size);
	UCadenceVariableInt* OutputSizeVar = OutputSizePin->GetVariable<UCadenceVariableInt>();

	OutputSizeVar->SetValue(InputArrayVar->GetSize());
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceArrayFindIndexNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
	AddInputVariableWildcardPin(FCadencePinConstants::Pin_Item);
}

void UCadenceArrayFindIndexNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Index, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayFindIndexNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* InputItemPin = GetInputPin(FCadencePinConstants::Pin_Item);
	UCadenceVariable* InputItemVar = InputItemPin->GetVariable();

	int32 ItemIndex = InputArrayVar->GetIndexOfElement(InputItemVar);
	
	UCadenceGraphNodePin* OutIndexPin = GetOutputPin(FCadencePinConstants::Pin_Index);
	UCadenceVariableInt* OutIndexVar = OutIndexPin->GetVariable<UCadenceVariableInt>();

	OutIndexVar->SetValue(ItemIndex);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceArrayContainsNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
	AddInputVariableWildcardPin(FCadencePinConstants::Pin_Item);
}

void UCadenceArrayContainsNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Value, UCadenceVariableBool::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayContainsNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* InputItemPin = GetInputPin(FCadencePinConstants::Pin_Item);
	UCadenceVariable* InputItemVar = InputItemPin->GetVariable();

	bool ItemExists = InputArrayVar->GetIndexOfElement(InputItemVar) != INDEX_NONE;
	
	UCadenceGraphNodePin* OutValuePin = GetOutputPin(FCadencePinConstants::Pin_Value);
	UCadenceVariableBool* OutValueVar = OutValuePin->GetVariable<UCadenceVariableBool>();

	OutValueVar->SetValue(ItemExists);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceArrayAddItemNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
	AddInputVariableWildcardPin(FCadencePinConstants::Pin_Item);
	AddInputVariablePinDefault<UCadenceVariableBool>(FCadencePinConstants::Pin_Unique, true);
}

void UCadenceArrayAddItemNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Index, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayAddItemNode::Execute(UCadenceContext* InContext)
{	
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* InputItemPin = GetInputPin(FCadencePinConstants::Pin_Item);
	UCadenceVariable* InputItemVar = InputItemPin->GetVariable();

	UCadenceGraphNodePin* InputUniquePin = GetInputPin(FCadencePinConstants::Pin_Unique);
	UCadenceVariableBool* InputUniqueVar = InputUniquePin->GetVariable<UCadenceVariableBool>();

	int32 Index = INDEX_NONE;
	if(InputUniqueVar->GetValue() == true)	
		Index = InputArrayVar->GetIndexOfElement(InputItemVar);

	if(Index == INDEX_NONE)
		Index = InputArrayVar->AddElement(InputItemVar);
	
	UCadenceGraphNodePin* OutIndexPin = GetOutputPin(FCadencePinConstants::Pin_Index);
	UCadenceVariableInt* OutIndexVar = OutIndexPin->GetVariable<UCadenceVariableInt>();

	OutIndexVar->SetValue(Index);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceArrayRemoveItemNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
	AddInputVariableWildcardPin(FCadencePinConstants::Pin_Item);
}

void UCadenceArrayRemoveItemNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Removed, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayRemoveItemNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* InputItemPin = GetInputPin(FCadencePinConstants::Pin_Item);
	UCadenceVariable* InputItemVar = InputItemPin->GetVariable();

	int32 RemovedCount = InputArrayVar->RemoveElement(InputItemVar);
	
	UCadenceGraphNodePin* OutRemovedPin = GetOutputPin(FCadencePinConstants::Pin_Removed);
	UCadenceVariableInt* OutRemovedVar = OutRemovedPin->GetVariable<UCadenceVariableInt>();

	OutRemovedVar->SetValue(RemovedCount);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceArrayRemoveByIndexNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array);
	AddInputVariablePinDefault<UCadenceVariableInt>(FCadencePinConstants::Pin_Index, 0);
}

void UCadenceArrayRemoveByIndexNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Removed, UCadenceVariableBool::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayRemoveByIndexNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* InputArrayPin = GetInputPin(FCadencePinConstants::Pin_Array);
	UCadenceVariableArray* InputArrayVar = InputArrayPin->GetVariable<UCadenceVariableArray>();
	
	UCadenceGraphNodePin* InputItemPin = GetInputPin(FCadencePinConstants::Pin_Item);
	UCadenceVariableInt* InputItemVar = InputItemPin->GetVariable<UCadenceVariableInt>();

	bool IsRemoved = InputArrayVar->RemoveElement(InputItemVar->GetValue());
	
	UCadenceGraphNodePin* OutRemovedPin = GetOutputPin(FCadencePinConstants::Pin_Removed);
	UCadenceVariableBool* OutRemovedVar = OutRemovedPin->GetVariable<UCadenceVariableBool>();

	OutRemovedVar->SetValue(IsRemoved);
	
	return ECadenceNodeExecuteResult::Complete;
}
