// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceArrayNodes.h"

#include "Cadence.h"

void UCadenceForEachNode::CreateInputPins()
{
	Super::CreateInputPins();
	UCadenceGraphNodePin* Pin = AddInputVariablePin(FCadencePinConstants::Pin_Array, UCadenceVariableArray::StaticClass());
	Pin->OnPinConnected.AddUObject(this, &UCadenceForEachNode::OnInputPinConnected);
	Pin->OnConnectionsCleared.AddUObject(this, &UCadenceForEachNode::OnInputPinConnectionsCleared);
}

void UCadenceForEachNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputExecPin(FCadencePinConstants::Pin_LoopBody);
	AddOutputVariablePin(FCadencePinConstants::Pin_ArrayElement, nullptr);
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

void UCadenceForEachNode::OnInputPinConnected(UCadenceGraphNodePin* InConnectedPin)
{
	UCadenceVariable* ConnectedVar = InConnectedPin->GetVariable(false);
	if(ensure(ConnectedVar))
	{
		UCadenceVariableArray* ConnectedArray = Cast<UCadenceVariableArray>(ConnectedVar);

		UCadenceGraphNodePin* ArrayInputPin = GetInputPin(FCadencePinConstants::Pin_Array);
		UCadenceVariableArray* ArrayInputVariable = ArrayInputPin->GetVariable<UCadenceVariableArray>();
		ArrayInputVariable->SetVariableClass(ConnectedArray->GetVariableClass());
		
		UCadenceGraphNodePin* ArrayElementOutputPin = GetOutputPin(FCadencePinConstants::Pin_ArrayElement);
		ArrayElementOutputPin->SetVariableClass(ConnectedArray->GetVariableClass());
	}
}

void UCadenceForEachNode::OnInputPinConnectionsCleared()
{	
	UCadenceGraphNodePin* ArrayElementOutputPin = GetOutputPin(FCadencePinConstants::Pin_ArrayElement);
	ArrayElementOutputPin->SetVariableClass(nullptr);
}
