// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceReactionNodes.h"

#include "CadenceSubsystem.h"


void UCadenceSetReactionGroupValuesNode::CreateInputPins()
{
	Super::CreateInputPins();

	if(ValueChangedVariable.IsValid())
	{
		ValueChangedVariable->OnValueChanged.RemoveDynamic(this, &UCadenceSetReactionGroupValuesNode::UpdateVariableInputPins);
	}
	
	UCadenceGraphNodePin* Pin = AddInputVariablePin(FCadencePinConstants::Pin_ReactionGroup, UCadenceVariableReactionGroup::StaticClass());
	ValueChangedVariable = Pin->GetVariable();
	ValueChangedVariable->OnValueChanged.AddUniqueDynamic(this, &UCadenceSetReactionGroupValuesNode::UpdateVariableInputPins);

	UpdateVariableInputPins();
}

void UCadenceSetReactionGroupValuesNode::CreateOutputPins()
{
	Super::CreateOutputPins();
}

ECadenceNodeExecuteResult UCadenceSetReactionGroupValuesNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* GroupPin = GetInputPin(FCadencePinConstants::Pin_ReactionGroup);
	if(!GroupPin)
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceVariableReactionGroup* GroupVariable = GroupPin->GetVariable<UCadenceVariableReactionGroup>();
	if(!GroupVariable)
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceReactionGroup* Group = GroupVariable->GetValue();
	if(!Group)
		return ECadenceNodeExecuteResult::Failed;
	
	UWorld* World = GetWorld();
	if(!ensure(World))
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceSubsystem* Subsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!ensure(Subsystem))
		return ECadenceNodeExecuteResult::Failed;

	UCadenceReactionGroup* GroupInstance = Subsystem->GetReactionGroupRuntimeInstance(Group);

	for(UCadenceVariable* GroupItemVariable : GroupInstance->GetVariables())
	{
		UCadenceGraphNodePin* Pin = GetInputPin(GroupItemVariable->GetUserVariableName());
		if(!Pin->HasConnections())
			continue;
		
		UCadenceVariable* PinVariable = Pin->GetVariable();

		GroupItemVariable->CopyValueFrom(PinVariable);
	}
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceSetReactionGroupValuesNode::UpdateVariableInputPins()
{
	UCadenceGraphNodePin* GroupPin = GetInputPin(FCadencePinConstants::Pin_ReactionGroup);
	if(!GroupPin)
		return;
	
	UCadenceVariableReactionGroup* GroupVariable = GroupPin->GetVariable<UCadenceVariableReactionGroup>();
	if(!GroupVariable)
		return;
	
	UCadenceReactionGroup* Group = GroupVariable->GetValue();
	if(!Group)
		return;
	
	TArray<TObjectPtr<UCadenceVariable>> InputVariables = Group->GetVariables();

	auto RemoveNotInInputList = [&] (UCadenceGraphNodePin* Pin)
	{
		return VariableInputPins.Contains(Pin) && !InputVariables.ContainsByPredicate([&Pin] (UCadenceVariable* Variable)
		{
			return Variable->GetUserVariableName() == Pin->GetPinName();
		});
	};

	InputPins.RemoveAll(RemoveNotInInputList);

	VariableInputPins.Empty();

	for(UCadenceVariable* InputVariable : InputVariables)
	{
		UCadenceGraphNodePin* Pin = AddInputVariablePinUnique(InputVariable->GetUserVariableName(), InputVariable->GetClass());
		Pin->SetDefaultDisabled(true);
		VariableInputPins.Add(Pin);
	}
}

void UCadenceBreakReactionGroupNode::CreateInputPins()
{
	Super::CreateInputPins();
	
	if(ValueChangedVariable.IsValid())
	{
		ValueChangedVariable->OnValueChanged.RemoveDynamic(this, &UCadenceBreakReactionGroupNode::UpdateVariableOutputPins);
	}
	
	UCadenceGraphNodePin* Pin = AddInputVariablePin(FCadencePinConstants::Pin_ReactionGroup, UCadenceVariableReactionGroup::StaticClass(), 1);
	ValueChangedVariable = Pin->GetVariable();
	ValueChangedVariable->OnValueChanged.AddUniqueDynamic(this, &UCadenceBreakReactionGroupNode::UpdateVariableOutputPins);
}

void UCadenceBreakReactionGroupNode::CreateOutputPins()
{
	Super::CreateOutputPins();	
	UpdateVariableOutputPins();
}

void UCadenceBreakReactionGroupNode::UpdateVariableOutputPins()
{
	UCadenceGraphNodePin* GroupPin = GetInputPin(FCadencePinConstants::Pin_ReactionGroup);
	if(!GroupPin)
		return;
	
	UCadenceVariableReactionGroup* GroupVariable = GroupPin->GetVariable<UCadenceVariableReactionGroup>();
	if(!GroupVariable)
		return;
	
	UCadenceReactionGroup* Group = GroupVariable->GetValue();
	if(!Group)
		return;
	
	TArray<TObjectPtr<UCadenceVariable>> OutputVariables = Group->GetVariables();

	auto RemoveNotInOutputList = [&] (UCadenceGraphNodePin* Pin)
	{
		return VariableOutputPins.Contains(Pin) && !OutputVariables.ContainsByPredicate([&Pin] (UCadenceVariable* Variable)
		{
			return Variable->GetUserVariableName() == Pin->GetPinName();
		});
	};

	OutputPins.RemoveAll(RemoveNotInOutputList);

	for(UCadenceVariable* OutputVariable : OutputVariables)
	{
		VariableOutputPins.Add(AddOutputVariablePinUnique(OutputVariable->GetUserVariableName(), OutputVariable->GetClass()));
	}
}

ECadenceNodeExecuteResult UCadenceBreakReactionGroupNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* GroupPin = GetInputPin(FCadencePinConstants::Pin_ReactionGroup);
	if(!GroupPin)
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceVariableReactionGroup* GroupVariable = GroupPin->GetVariable<UCadenceVariableReactionGroup>();
	if(!GroupVariable)
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceReactionGroup* Group = GroupVariable->GetValue();
	if(!Group)
		return ECadenceNodeExecuteResult::Failed;
	
	UWorld* World = GetWorld();
	if(!ensure(World))
		return ECadenceNodeExecuteResult::Failed;
	
	UCadenceSubsystem* Subsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!ensure(Subsystem))
		return ECadenceNodeExecuteResult::Failed;

	UCadenceReactionGroup* GroupInstance = Subsystem->GetReactionGroupRuntimeInstance(Group);

	for(UCadenceVariable* GroupItemVariable : GroupInstance->GetVariables())
	{
		UCadenceGraphNodePin* Pin = GetInputPin(GroupItemVariable->GetUserVariableName());
		UCadenceVariable* PinVariable = Pin->GetVariable();

		PinVariable->CopyValueFrom(GroupItemVariable);
	}
	
	return ECadenceNodeExecuteResult::Complete;
}
