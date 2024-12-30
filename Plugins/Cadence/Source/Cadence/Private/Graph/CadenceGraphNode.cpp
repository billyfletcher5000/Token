// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraphNode.h"

#include "Algo/ForEach.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"

void UCadenceGraphNode::PostLoad()
{
	UObject::PostLoad();
	RebuildAndValidateWildcardToVariableClass();
}

void UCadenceGraphNode::GenerateGUID()
{
	GUID = FGuid::NewGuid();
	DebugName = GetName();
}

void UCadenceGraphNode::DestroyNode()
{
	Modify();
	
	for(UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	for(UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();

	InputPins.Empty();
	OutputPins.Empty();
	
	ParentGraph->RemoveNode(this);
}

void UCadenceGraphNode::CreateInputPins()
{
	if(!IsPure() && HasInputExecPin())
	{
		AddInputExecPin(FCadencePinConstants::Pin_Default_Exec);
	}
}

void UCadenceGraphNode::CreateOutputPins()
{
	if(!IsPure() && ShouldCreateThenExecPin())
	{
		AddOutputExecPin(FCadencePinConstants::Pin_Default_Then);
	}
}

void UCadenceGraphNode::ClearConnections()
{
	for(UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	for(UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetExecPin() const
{
	return GetInputPin(FCadencePinConstants::Pin_Default_Exec);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetThenPin() const
{
	return GetOutputPin(FCadencePinConstants::Pin_Default_Then);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetInputPin(const FName& InPinName) const
{
	return GetPinFromArray(InputPins, InPinName);
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetOutputPin(const FName& InPinName) const
{
	return GetPinFromArray(OutputPins, InPinName);
}

UCadenceGraphNodePin* UCadenceGraphNode::GetMostAppropriateAutomaticInputPin(UCadenceGraphNodePin* OtherPin)
{
	for(UCadenceGraphNodePin* MyPin : InputPins)
	{
		if(MyPin->IsExec() && OtherPin->IsExec())
			return MyPin;

		if(MyPin->GetVariableClass() == OtherPin->GetVariableClass())
			return MyPin;
	}

	return nullptr;
}

UCadenceGraphNodePin* UCadenceGraphNode::GetMostAppropriateAutomaticOutputPin(UCadenceGraphNodePin* OtherPin)
{
	for(UCadenceGraphNodePin* MyPin : OutputPins)
	{
		if(MyPin->IsExec() && OtherPin->IsExec())
			return MyPin;

		if(MyPin->GetVariableClass() == OtherPin->GetVariableClass())
			return MyPin;
	}

	return nullptr;
}

bool UCadenceGraphNode::HasAnyPinsWithWildcard(const int32& InWildcardId)
{
	for (UCadenceGraphNodePin* Pin : InputPins)
	{
		if(Pin->GetWildcardId() == InWildcardId)
			return true;
	}
	
	for (UCadenceGraphNodePin* Pin : OutputPins)
	{
		if(Pin->GetWildcardId() == InWildcardId)
			return true;
	}

	return false;
}

TArray<UCadenceGraphNodePin*> UCadenceGraphNode::GetPinsWithWildcard(const int32& InWildcardId)
{
	TArray<UCadenceGraphNodePin*> OutPins;
	auto CollectWildcardPins = [&InWildcardId, &OutPins] (UCadenceGraphNodePin* Pin)
	{
		if(Pin->GetWildcardId() == InWildcardId)		
			OutPins.AddUnique(Pin);		
	};
	
	Algo::ForEach(InputPins, CollectWildcardPins);
	Algo::ForEach(OutputPins, CollectWildcardPins);

	return OutPins;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputExecPin(const FName& InPinName, const int32& InIndex)
{
	ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateExecPin(InPinName);
	AddPinToList(Pin, InputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputExecPin(const FName& InPinName, const int32& InIndex)
{
	ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin"));
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateExecPin(InPinName);
	AddPinToList(Pin, OutputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const int32& InIndex)
{	
	if(!ensureMsgf(GetInputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin")))
		return nullptr;
		
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	AddPinToList(Pin, InputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariablePinUnique(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const int32& InIndex)
{
	UCadenceGraphNodePin* Pin = GetInputPin(InPinName);
	if(Pin)
	{
		if(Pin->GetVariableClass() != InVariableClass)
			Pin->SetVariableClass(InVariableClass);

		if(InIndex >= 0 && InIndex < InputPins.Num())
		{
			InputPins.Remove(Pin);
			InputPins.Insert(Pin, InIndex);
		}

		return Pin;
	}
	else
	{
		return AddInputVariablePin(InPinName, InVariableClass, InIndex);
	}
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const int32& InIndex)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId);
	AddPinToList(Pin, InputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddInputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId, const int32& InIndex)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, true);
	AddPinToList(Pin, InputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const int32& InIndex)
{
	if(!ensureMsgf(GetOutputPin(InPinName) == nullptr, TEXT("Cannot add pin with same name as existing pin")))
		return nullptr;
		
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariablePin(InPinName, InVariableClass);
	AddPinToList(Pin, OutputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariablePinUnique(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const int32& InIndex)
{
	UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
	if(Pin)
	{
		if(Pin->GetVariableClass() != InVariableClass)
			Pin->SetVariableClass(InVariableClass);

		if(InIndex >= 0 && InIndex < OutputPins.Num())
		{
			OutputPins.Remove(Pin);
			OutputPins.Insert(Pin, InIndex);
		}

		return Pin;
	}
	else
	{
		return AddOutputVariablePin(InPinName, InVariableClass, InIndex);
	}
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const int32& InIndex)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, false);
	AddPinToList(Pin, OutputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::AddOutputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId, const int32& InIndex)
{
	TObjectPtr<UCadenceGraphNodePin> Pin = CreateVariableWildcardPin(InPinName, InWildcardId, true);
	AddPinToList(Pin, OutputPins, InIndex);
	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateExecPin(const FName& InPinName)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(true);
	Pin->GenerateGUID();
	Pin->SetFlags(RF_Transactional);

	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateVariablePin(const FName& InPinName, const TObjectPtr<UClass>& InVariableClass, const bool InIsArray)
{	
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(false);
	Pin->SetVariableClass(InVariableClass);
	Pin->GenerateGUID();
	Pin->SetFlags(RF_Transactional);

	return Pin;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::CreateVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const bool InIsArray)
{
	int32 WildcardId = InWildcardId;
	if(!ensureMsgf(WildcardId != -1, TEXT("Cannot use -1 as wildcard ID, reserved for initialisation purposes")))
		WildcardId = 1234567;
	UCadenceGraphNodePin* Pin = NewObject<UCadenceGraphNodePin>(this);

	Pin->SetParentNode(this);
	Pin->SetPinName(InPinName);
	Pin->SetIsExec(false);
	Pin->SetVariableClass(InIsArray ? UCadenceVariableArray::StaticClass() : nullptr);
	Pin->SetWildcardId(WildcardId);
	Pin->GenerateGUID();
	Pin->SetFlags(RF_Transactional);

	Pin->OnPinConnected.AddUObject(this, &UCadenceGraphNode::OnPinConnectedToWildcardPin, Pin);
	Pin->OnConnectionsCleared.AddUObject(this, &UCadenceGraphNode::OnPinConnectionsClearedFromWildcardPin, Pin);	
	
	return Pin;
}

bool UCadenceGraphNode::RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(InputPins.Contains(InPin))
	{
		InPin->ClearConnections();
		InputPins.Remove(InPin);
		return true;
	}

	return false;
}

bool UCadenceGraphNode::RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin)
{
	if(OutputPins.Contains(InPin))
	{
		InPin->ClearConnections();
		OutputPins.Remove(InPin);
		return true;
	}

	return false;
}

void UCadenceGraphNode::RemoveAllInputPins()
{
	for (UCadenceGraphNodePin* Pin : InputPins)
		Pin->ClearConnections();

	InputPins.Empty();
}

void UCadenceGraphNode::RemoveAllOutputPins()
{
	for (UCadenceGraphNodePin* Pin : OutputPins)
		Pin->ClearConnections();

	OutputPins.Empty();
}

void UCadenceGraphNode::RemoveAllPins()
{
	RemoveAllInputPins();
	RemoveAllOutputPins();
}

TObjectPtr<UCadenceGraphNodePin> UCadenceGraphNode::GetPinFromArray(const TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName)
{
	auto Result = InPinArray.FindByPredicate([&InPinName](const TObjectPtr<UCadenceGraphNodePin>& Pin)
	{
		return Pin->GetPinName() == InPinName;
	});

	if(Result != nullptr)
		return *Result;

	return nullptr;
}

void UCadenceGraphNode::AddPinToList(UCadenceGraphNodePin* InPin, TArray<TObjectPtr<UCadenceGraphNodePin>>& InList,	const int32& InIndex)
{
	if(InIndex >= 0 && InIndex < InList.Num())
	{
		InList.Insert(InPin, InIndex);
	}
	else
	{
		InList.Add(InPin);
	}
}

void UCadenceGraphNode::RebuildAndValidateWildcardToVariableClass()
{
	WildcardIdToVariableClass.Empty();
	TMap<int32, TArray<UCadenceGraphNodePin*>> WildcardIdToPinArray;

	auto GatherWildcards = [&WildcardIdToPinArray] (UCadenceGraphNodePin* Pin)
	{
		int32 WildcardId = Pin->GetWildcardId();
		if(WildcardId != -1)
		{
			if(WildcardIdToPinArray.Contains(WildcardId))
			{
				TArray<UCadenceGraphNodePin*>& PinArray = WildcardIdToPinArray[WildcardId];
				PinArray.AddUnique(Pin);
			}
			else
			{
				TArray<UCadenceGraphNodePin*> PinArray = { Pin };
				WildcardIdToPinArray.Add(WildcardId, PinArray);
			}
		}
	};
	
	Algo::ForEach(InputPins, GatherWildcards);
	Algo::ForEach(OutputPins, GatherWildcards);

	for (TTuple<int32, TArray<UCadenceGraphNodePin*>>& IdPinArrayPair : WildcardIdToPinArray)
	{
		for (UCadenceGraphNodePin* Pin : IdPinArrayPair.Value)
		{
			if(UClass* PinVariableClass = Pin->GetVariableClass())
			{
				if(PinVariableClass == UCadenceVariableArray::StaticClass())
				{
					UCadenceVariableArray* VariableArray = Pin->GetVariable<UCadenceVariableArray>(false);
					if(!ensure(VariableArray))
						continue;

					PinVariableClass = VariableArray->GetVariableClass();
				}

				int32 WildcardId = IdPinArrayPair.Key;
				
				if(WildcardIdToVariableClass.Contains(WildcardId))
				{
					UClass* PreviousStoredClass = WildcardIdToVariableClass[WildcardId];
					ensureMsgf(PinVariableClass == PreviousStoredClass, TEXT("Pins with Wildcard ID \"%d\" have type conflict: (%s, %s)"), WildcardId, *PinVariableClass->GetName(), *PreviousStoredClass->GetName());
				}
				else
				{
					WildcardIdToVariableClass.Add(WildcardId, PinVariableClass);
				}
			}
		}
	}
}

void UCadenceGraphNode::OnPinConnectedToWildcardPin(UCadenceGraphNodePin* InConnectedPin, UCadenceGraphNodePin* InWildcardPin)
{
	if(!ensure(InConnectedPin))
		return;

	UClass* TargetClass = nullptr;
	if(InConnectedPin->GetVariableClass() == UCadenceVariableArray::StaticClass())
	{
		UCadenceVariable* ConnectedVar = InConnectedPin->GetVariable(false);
		if(ensure(ConnectedVar))
		{
			UCadenceVariableArray* ConnectedArray = Cast<UCadenceVariableArray>(ConnectedVar);
			TargetClass = ConnectedArray->GetVariableClass();
		}
	}
	else
	{
		TargetClass = InConnectedPin->GetVariableClass();
	}

	if(!ensure(TargetClass))
		return;

	int32 WildcardId = InWildcardPin->GetWildcardId();

	if(WildcardIdToVariableClass.Contains(WildcardId))
	{
		UClass* ExistingClass = WildcardIdToVariableClass[WildcardId];
		ensureMsgf(TargetClass == ExistingClass, TEXT("Pins exist with wildcard variable class that differs from new connection variable class: Existing: %s New: %s"), *ExistingClass->GetName(), *TargetClass->GetName());
		return;
	}

	auto WildcardPins = GetPinsWithWildcard(WildcardId);

	auto SetClassLambda = [&TargetClass] (UCadenceGraphNodePin* Pin)
	{
		if(Pin->GetVariableClass() == UCadenceVariableArray::StaticClass())
		{
			UCadenceVariableArray* PinArray = Pin->GetVariable<UCadenceVariableArray>();
			PinArray->SetVariableClass(TargetClass);
		}
		else
		{
			Pin->SetVariableClass(TargetClass);
		}		
	};
	
	Algo::ForEach(WildcardPins, SetClassLambda);
	WildcardIdToVariableClass.Add(WildcardId, TargetClass);

#if WITH_EDITOR
	GetParentGraph()->NotifyPinTypesChanged();
#endif
}

void UCadenceGraphNode::OnPinConnectionsClearedFromWildcardPin(UCadenceGraphNodePin* InWildcardPin)
{
	int32 WildcardId = InWildcardPin->GetWildcardId();

	TArray<UCadenceGraphNodePin*> AllWildcardPins = GetPinsWithWildcard(WildcardId);

	for (UCadenceGraphNodePin* WildcardPin : AllWildcardPins)
	{
		if(WildcardPin->HasConnections())
			return;
	}

	auto SetClassLambda = [](UCadenceGraphNodePin* Pin)
	{
		if(Pin->GetVariableClass() == UCadenceVariableArray::StaticClass())
		{
			UCadenceVariableArray* PinArray = Pin->GetVariable<UCadenceVariableArray>();
			PinArray->SetVariableClass(nullptr);
		}
		else
		{
			Pin->SetVariableClass(nullptr);
		}
	};
	
	Algo::ForEach(AllWildcardPins, SetClassLambda);	
	WildcardIdToVariableClass.Remove(WildcardId);

#if WITH_EDITOR
	GetParentGraph()->NotifyPinTypesChanged();
#endif
}
