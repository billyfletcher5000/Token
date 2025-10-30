// Copyright (C) Billy Fletcher 2025


#include "Reaction/CadenceReactionGroupFunctionLibrary.h"

#include "CadenceSubsystem.h"
#include "Graph/CadenceVariable.h"
#include "Reaction/CadenceReactionGroup.h"

UCadenceVariable* UCadenceReactionGroupFunctionLibrary::GetReactionGroupVariable(UObject* InInstigator, UCadenceReactionGroup* InReactionGroup, FName VariableName)
{
	if(!ensure(InInstigator) || !ensure(InReactionGroup))
		return nullptr;
	
	UWorld* World = InInstigator->GetWorld();
	if(!ensure(World))
		return nullptr;
	
	UCadenceSubsystem* Subsystem = World->GetSubsystem<UCadenceSubsystem>();
	if(!ensure(Subsystem))
		return nullptr;

	UCadenceReactionGroup* GroupInstance = Subsystem->GetReactionGroupRuntimeInstance(InReactionGroup);
	
	TArray<TObjectPtr<UCadenceVariable>> const& Variables = GroupInstance->GetVariables();
	const TObjectPtr<UCadenceVariable>* Result = Variables.FindByPredicate([&VariableName] (UCadenceVariable* InVar)
	{
		return InVar->GetUserVariableName() == VariableName;
	});

	if(Result == nullptr)
		return nullptr;

	return *Result;
}
