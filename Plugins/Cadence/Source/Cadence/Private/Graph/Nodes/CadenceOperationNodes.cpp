// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceOperationNodes.h"

#include "Cadence.h"


const int32 UCadenceOperationNode_Base::PrimaryWildcardId = 0;
const int32 UCadenceOperationNode_Base::SecondaryWildcardId = 1;
const int32 UCadenceOperationNode_Base::ResultWildcardId = 2;

void UCadenceOperationNode_Base::CreateInputPins()
{
	Super::CreateInputPins();
	RefreshInputPins();
}

void UCadenceOperationNode_Base::CreateOutputPins()
{
	Super::CreateOutputPins();
	RefreshOutputPins();
}

ECadenceNodeExecuteResult UCadenceOperationNode_Base::Execute(UCadenceContext* InContext)
{
	if(!ensure(PrimaryInputPin.IsValid()))
		return ECadenceNodeExecuteResult::Failed;

	if(!ensure(!SecondaryInputPins.IsEmpty()))
		return ECadenceNodeExecuteResult::Failed;

	if(!ensure(ResultOutputPin.IsValid()))
		return ECadenceNodeExecuteResult::Failed;

	if(!ensure(IsValid(Operation)))
		return ECadenceNodeExecuteResult::Failed;

	UCadenceVariable* PrimaryVariable = PrimaryInputPin->GetVariable();
	if(!ensure(IsValid(PrimaryVariable)))
		return ECadenceNodeExecuteResult::Failed;

	TArray<UCadenceVariable*> SecondaryVariables;
	for(TWeakObjectPtr<UCadenceGraphNodePin>& SecondaryPin : SecondaryInputPins)
	{
		ensure(SecondaryPin.IsValid());
		UCadenceVariable* SecondaryVariable = SecondaryPin->GetVariable();
		if(ensure(SecondaryVariable))
			SecondaryVariables.Add(SecondaryVariable);		
	}

	UCadenceVariable* OutputVariable = ResultOutputPin->GetVariable();
	if(!ensure(IsValid(OutputVariable)))
		return ECadenceNodeExecuteResult::Failed;

	if(!ensure(Operation->ApplyOperation(PrimaryVariable, SecondaryVariables, OutputVariable)))
		return ECadenceNodeExecuteResult::Failed;
	
	return ECadenceNodeExecuteResult::Complete;
}

UCadenceGraphNodePin* UCadenceOperationNode_Base::AddUserInputPin()
{
	return AddSecondaryInputPin();
}

bool UCadenceOperationNode_Base::RemoveUserInputPin(UCadenceGraphNodePin* Pin)
{
	if(RemoveInputPin(Pin))
	{
		SecondaryInputPins.Remove(Pin);
		return true;
	}

	return false;
}

bool UCadenceOperationNode_Base::CanRemovePin(const UCadenceGraphNodePin* Pin) const
{
	return SecondaryInputPins.Contains(Pin);
}

#if WITH_EDITOR
bool UCadenceOperationNode_Base::SetOperation(UCadenceOperation* InOperation, const bool& bInIsReversed)
{
	if(Operation != InOperation || (Operation != nullptr && bIsOperationReversed != bInIsReversed))
	{
		Operation = InOperation;
		bIsOperationReversed = bInIsReversed;
		return RefreshPins();
	}

	return false;
}

UClass* UCadenceOperationNode_Base::GetPrimaryVariableType(const bool& InRequireConnection) const
{
	if(!PrimaryInputPin.IsValid() || !HasAnyConnections())
		return nullptr;

	if(InRequireConnection && !PrimaryInputPin->HasConnections())
		return nullptr;
	
	return PrimaryInputPin->GetVariableClass();
}

UClass* UCadenceOperationNode_Base::GetSecondaryVariableType(const bool& InRequireConnection) const
{
	if(!HasAnyConnections())
		return nullptr;
	
	for(auto& Pin : SecondaryInputPins)
	{
		if(Pin.IsValid() && (!InRequireConnection || Pin->HasConnections()))
			return Pin->GetVariableClass();
	}

	return nullptr;
}

UClass* UCadenceOperationNode_Base::GetResultVariableType(const bool& InRequireConnection) const
{	
	if(!ResultOutputPin.IsValid() || !HasAnyConnections())
		return nullptr;

	if(InRequireConnection && !ResultOutputPin->HasConnections())
		return nullptr;
	
	return ResultOutputPin->GetVariableClass();
}
#endif

bool UCadenceOperationNode_Base::RefreshPins()
{
	if(RefreshInputPins() || RefreshOutputPins())
		return true;
	
	return false;
}

bool UCadenceOperationNode_Base::RefreshInputPins()
{
	bool bAnyChanges = false;
	if(IsValid(Operation))
	{
		UClass* PrimaryVariableClass = GetPrimaryVariableClass();
		if(PrimaryInputPin.IsValid())
		{
			if(PrimaryInputPin->GetVariableClass() != PrimaryVariableClass)
			{
				// Check for existing secondary pin that matches the primary type and is the only connected secondary pin
				if(!PrimaryInputPin->HasConnections())
				{
					TWeakObjectPtr<UCadenceGraphNodePin> SwapCandidate = nullptr;
					for(TWeakObjectPtr<UCadenceGraphNodePin>& SecondaryPin : SecondaryInputPins)
					{
						if(SecondaryPin->HasConnections() && SecondaryPin->GetVariableClass() == PrimaryVariableClass)
						{
							if(SwapCandidate != nullptr)
							{
								SwapCandidate = nullptr;
								break;								
							}

							SwapCandidate = SecondaryPin;
						}
					}

					if(SwapCandidate.IsValid())
					{
						UE_LOG(LogCadence, Log, TEXT("Swapping SecondaryPin \"%s\" to: nullptr"), *PrimaryInputPin->GetPinName().ToString());
						PrimaryInputPin->SetVariableClass(nullptr);
						SecondaryInputPins.Add(PrimaryInputPin);
						PrimaryInputPin = SwapCandidate;
					}
				}

				UE_LOG(LogCadence, Log, TEXT("Setting PrimaryPin to: %s"), *PrimaryVariableClass->GetFName().ToString());				
				PrimaryInputPin->SetVariableClass(PrimaryVariableClass);
				bAnyChanges = true;
			}
		}
		else
		{
			PrimaryInputPin = AddInputVariableWildcardPin(FCadencePinConstants::Pin_Primary, PrimaryAllowedTypes, PrimaryWildcardId);
			PrimaryInputPin->SetShouldHidePinName(true);
			PrimaryInputPin->SetVariableClass(GetPrimaryVariableClass());
			UE_LOG(LogCadence, Log, TEXT("Creating PrimaryPin with: %s"), *PrimaryVariableClass->GetFName().ToString());	
			bAnyChanges = true;
		}
	}
	else
	{
		if(PrimaryInputPin.IsValid())
		{
			if(!PrimaryInputPin->HasConnections() && PrimaryInputPin->GetVariableClass() != nullptr)
			{
				UE_LOG(LogCadence, Log, TEXT("Setting PrimaryPin to: nullptr"));
				PrimaryInputPin->SetVariableClass(nullptr);
				bAnyChanges = true;
			}
		}
		else
		{
			PrimaryInputPin = AddInputVariableWildcardPin(FCadencePinConstants::Pin_Primary, PrimaryAllowedTypes, PrimaryWildcardId);
			PrimaryInputPin->SetShouldHidePinName(true);
			UE_LOG(LogCadence, Log, TEXT("Creating PrimaryPin (wildcard)"));
			bAnyChanges = true;
		}
	}
	
	bAnyChanges |= SecondaryInputPins.RemoveAll([] (const TWeakObjectPtr<UCadenceGraphNodePin>& InItem) { return InItem == nullptr; }) > 0;
	
	if(IsValid(Operation))
	{
		UClass* SecondaryVariableClass = GetSecondaryVariableClass();
		for(TWeakObjectPtr<UCadenceGraphNodePin>& SecondaryPin : SecondaryInputPins)
		{
			if(!SecondaryPin->HasConnections() && SecondaryPin->GetVariableClass() != SecondaryVariableClass)
			{
				UE_LOG(LogCadence, Log, TEXT("Setting SecondaryPin \"%s\" to: %s"), *SecondaryPin->GetPinName().ToString(), *SecondaryVariableClass->GetFName().ToString());
				SecondaryPin->SetVariableClass(SecondaryVariableClass);
				bAnyChanges = true;
			}
		}
	}
	else
	{
		for(TWeakObjectPtr<UCadenceGraphNodePin>& SecondaryPin : SecondaryInputPins)
		{
			if(!SecondaryPin->HasConnections() && SecondaryPin->GetVariableClass() != nullptr)
			{
				UE_LOG(LogCadence, Log, TEXT("Setting SecondaryPin \"%s\" to: nullptr"), *SecondaryPin->GetPinName().ToString());
				SecondaryPin->SetVariableClass(nullptr);
				bAnyChanges = true;
			}
		}
	}

	SecondaryInputPins.Sort([this](const TWeakObjectPtr<UCadenceGraphNodePin>& InA, const TWeakObjectPtr<UCadenceGraphNodePin>& InB)
	{
		return GetInputPinIndex(InA.Get()) < GetInputPinIndex(InB.Get());
	});

	if(SecondaryInputPins.Num() == 0)
	{
		AddSecondaryInputPin();
		bAnyChanges = true;
	}

	return bAnyChanges;
}

bool UCadenceOperationNode_Base::RefreshOutputPins()
{
	bool bAnyChanges = false;

	if(IsValid(Operation))
	{
		TSubclassOf<UCadenceVariable> ResultType = Operation->GetResultType();
		if(ResultOutputPin.IsValid())
		{
			if(ResultOutputPin->GetVariableClass() != ResultType)
			{
				UE_LOG(LogCadence, Log, TEXT("Setting ResultPin to: %s"), *ResultType->GetFName().ToString());	
				ResultOutputPin->SetVariableClass(ResultType);
				bAnyChanges = true;
			}
		}
		else
		{
			ResultOutputPin = AddOutputVariableWildcardPin(FCadencePinConstants::Pin_Result, ResultAllowedTypes, ResultWildcardId);
			ResultOutputPin->SetShouldHidePinName(true);
			ResultOutputPin->SetVariableClass(ResultType);
			UE_LOG(LogCadence, Log, TEXT("Creating ResultPin with: %s"), *ResultType->GetFName().ToString());	
			bAnyChanges = true;
		}
	}
	else
	{
		if(ResultOutputPin.IsValid())
		{
			if(!ResultOutputPin->HasConnections() && ResultOutputPin->GetVariableClass() != nullptr)
			{
				ResultOutputPin->SetVariableClass(nullptr);
				UE_LOG(LogCadence, Log, TEXT("Setting ResultPin to: nullptr"));
				bAnyChanges = true;
			}
		}
		else
		{
			ResultOutputPin = AddOutputVariableWildcardPin(FCadencePinConstants::Pin_Result, ResultAllowedTypes, PrimaryWildcardId);
			ResultOutputPin->SetShouldHidePinName(true);			
			UE_LOG(LogCadence, Log, TEXT("Creating ResultPin (wildcard)"));
			bAnyChanges = true;
		}
	}
	
	return bAnyChanges;
}

TObjectPtr<UCadenceGraphNodePin> UCadenceOperationNode_Base::AddSecondaryInputPin()
{
	FString PinName = FCadencePinConstants::Pin_Secondary.ToString() + " " + GetNameForAdditionalPin(PinIndex).ToString();
	PinIndex++;

	UCadenceGraphNodePin* Pin = AddInputVariableWildcardPin(FName(PinName), SecondaryWildcardId);
	UClass* SecondaryVariableClass = GetSecondaryVariableClass();
	Pin->SetVariableClass(SecondaryVariableClass);
	Pin->SetShouldHidePinName(true);
	SecondaryInputPins.Add(Pin);

	FString ClassName = SecondaryVariableClass != nullptr ? SecondaryVariableClass->GetFName().ToString() : TEXT("nullptr");
	
	UE_LOG(LogCadence, Log, TEXT("Creating SecondaryPin \"%s\" with: %s"), *PinName, *ClassName);
	
	return Pin;
}

UClass* UCadenceOperationNode_Base::GetPrimaryVariableClass() const
{
	return IsValid(Operation) ? (bIsOperationReversed ? Operation->GetSecondaryType() : Operation->GetPrimaryType()) : nullptr;
}

UClass* UCadenceOperationNode_Base::GetSecondaryVariableClass() const
{
	return IsValid(Operation) ? (bIsOperationReversed ? Operation->GetPrimaryType() : Operation->GetSecondaryType()) : nullptr;
}

bool UCadenceOperationNode_Base::HasAnyConnections() const
{
	if(PrimaryInputPin->HasConnections())
		return true;

	if(ResultOutputPin->HasConnections())
		return true;

	for(const TWeakObjectPtr<UCadenceGraphNodePin>& SecondaryPin : SecondaryInputPins)
	{
		if(SecondaryPin->HasConnections())
			return true;
	}
	
	return false;
}
