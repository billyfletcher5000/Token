// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceArrayNodes.h"

#include "Cadence.h"

void UCadenceForEachNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array, 0);
}

void UCadenceForEachNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputExecPin(FCadencePinConstants::Pin_LoopBody);
	AddOutputVariableWildcardPin(FCadencePinConstants::Pin_ArrayElement, 0);
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

	//UE_LOG(LogCadence, Log, TEXT("ForEach: %d: %s"), Index, *ArrayElementOutputVariable->ConvertToValueString());
	
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
	AddInputVariablePin(FCadencePinConstants::Pin_Array, UCadenceVariableArray::StaticClass());
}

void UCadenceArraySizeNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceArraySizeNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

void UCadenceArrayFindIndexNode::CreateInputPins()
{
	Super::CreateInputPins();
	AddInputVariableWildcardArrayPin(FCadencePinConstants::Pin_Array, 0);
	AddInputVariableWildcardPin(FCadencePinConstants::Pin_Item, 0);
}

void UCadenceArrayFindIndexNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Index, UCadenceVariableInt::StaticClass());
}

ECadenceNodeExecuteResult UCadenceArrayFindIndexNode::Execute(UCadenceContext* InContext)
{
	
	
	UCadenceGraphNodePin* OutIndexPin = GetOutputPin(FCadencePinConstants::Pin_Index);
	UCadenceVariableInt* OutIndexVar = OutIndexPin->GetVariable<UCadenceVariableInt>();
	
	return Super::Execute(InContext);
}

void UCadenceArrayContainsNode::CreateInputPins()
{
	Super::CreateInputPins();
}

void UCadenceArrayContainsNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceArrayContainsNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

void UCadenceArrayAddItemNode::CreateInputPins()
{
	Super::CreateInputPins();
}

void UCadenceArrayAddItemNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceArrayAddItemNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

void UCadenceArrayRemoveItemNode::CreateInputPins()
{
	Super::CreateInputPins();
}

void UCadenceArrayRemoveItemNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceArrayRemoveItemNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}

void UCadenceArrayRemoveByIndexNode::CreateInputPins()
{
	Super::CreateInputPins();
}

void UCadenceArrayRemoveByIndexNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceArrayRemoveByIndexNode::Execute(UCadenceContext* InContext)
{
	return Super::Execute(InContext);
}
