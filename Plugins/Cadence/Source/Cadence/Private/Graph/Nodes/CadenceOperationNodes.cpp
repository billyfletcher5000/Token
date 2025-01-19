// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceOperationNodes.h"


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

UClass* UCadenceOperationNode_Base::GetPrimaryVariableType() const
{
	if(!PrimaryInputPin.IsValid() || !PrimaryInputPin->HasConnections())
		return nullptr;

	return PrimaryInputPin->GetVariableClass();
}

UClass* UCadenceOperationNode_Base::GetSecondaryVariableType() const
{
	for(auto& Pin : SecondaryInputPins)
	{
		if(Pin.IsValid() && Pin->HasConnections())
			return Pin->GetVariableClass();
	}

	return nullptr;
}

UClass* UCadenceOperationNode_Base::GetResultVariableType() const
{	
	if(!ResultOutputPin.IsValid() || !ResultOutputPin->HasConnections())
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
						PrimaryInputPin->SetVariableClass(nullptr);
						SecondaryInputPins.Add(PrimaryInputPin);
						PrimaryInputPin = SwapCandidate;
					}
				}
				
				PrimaryInputPin->SetVariableClass(PrimaryVariableClass);
				bAnyChanges = true;
			}
		}
		else
		{
			PrimaryInputPin = AddInputVariableWildcardPin(FCadencePinConstants::Pin_Primary, PrimaryAllowedTypes, PrimaryWildcardId);
			PrimaryInputPin->SetShouldHidePinName(true);
			PrimaryInputPin->SetVariableClass(GetPrimaryVariableClass());
			bAnyChanges = true;
		}
	}
	else
	{
		if(PrimaryInputPin.IsValid())
		{
			if(!PrimaryInputPin->HasConnections() && PrimaryInputPin->GetVariableClass() != nullptr)
			{
				PrimaryInputPin->SetVariableClass(nullptr);
				bAnyChanges = true;
			}
		}
		else
		{
			PrimaryInputPin = AddInputVariableWildcardPin(FCadencePinConstants::Pin_Primary, PrimaryAllowedTypes, PrimaryWildcardId);
			PrimaryInputPin->SetShouldHidePinName(true);
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
				ResultOutputPin->SetVariableClass(ResultType);
				bAnyChanges = true;
			}
		}
		else
		{
			ResultOutputPin = AddOutputVariableWildcardPin(FCadencePinConstants::Pin_Result, ResultAllowedTypes, ResultWildcardId);
			ResultOutputPin->SetShouldHidePinName(true);
			ResultOutputPin->SetVariableClass(ResultType);
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
				bAnyChanges = true;
			}
		}
		else
		{
			ResultOutputPin = AddOutputVariableWildcardPin(FCadencePinConstants::Pin_Result, ResultAllowedTypes, PrimaryWildcardId);
			ResultOutputPin->SetShouldHidePinName(true);
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
	Pin->SetVariableClass(GetSecondaryVariableClass());
	Pin->SetShouldHidePinName(true);
	SecondaryInputPins.Add(Pin);
	
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