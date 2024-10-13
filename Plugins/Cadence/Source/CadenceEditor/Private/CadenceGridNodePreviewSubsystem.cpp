// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGridNodePreviewSubsystem.h"

#include "CadenceGraphGridPreviewCommandDecorators.h"
#include "CadenceGraphPropertyCustomization.h"

void UCadenceGridNodePreviewSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<UClass*> PreviewDrawCommandDecoratorClasses;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		if (Class->IsChildOf(UCadenceGraphGridPreviewCommandDecorator::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
		{
			PreviewDrawCommandDecoratorClasses.Add(Class);
		}
	}

	for(UClass* DecoratorClass : PreviewDrawCommandDecoratorClasses)
	{
		UCadenceGraphGridPreviewCommandDecorator* CDO = DecoratorClass->GetDefaultObject<UCadenceGraphGridPreviewCommandDecorator>();

		UScriptStruct* TargetCommandType = CDO->GetTargetCommandType();

		if(CommandTypeToDecorator.Contains(TargetCommandType))
		{
			UE_LOG(LogCadenceEditor, Error, TEXT("Cannot use decorator %s as command decorator already exists for type: %s"), *DecoratorClass->GetName(), *TargetCommandType->GetName());
		}
		else
		{
			CommandTypeToDecorator[TargetCommandType] = CDO;
		}
	}
}

void UCadenceGridNodePreviewSubsystem::Deinitialize()
{
	Super::Deinitialize();

	CommandTypeToDecorator.Empty();
}

UCadenceGraphGridPreviewCommandDecorator* UCadenceGridNodePreviewSubsystem::GetDecoratorForCommandType(UScriptStruct* InCommandType) const
{
	if(CommandTypeToDecorator.Contains(InCommandType))
		return CommandTypeToDecorator[InCommandType];

	return nullptr;
}
