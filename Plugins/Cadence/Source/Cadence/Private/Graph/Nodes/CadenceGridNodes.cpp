// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceGridNodes.h"

#include "Cadence.h"
#include "EngineUtils.h"
#include "Actors/CadenceActorSettings.h"
#include "Actors/CadenceBlockGridActor.h"
#include "Actors/CadenceMeshSplineActor.h"
#include "Components/SplineComponent.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"

const FName UCadenceGridCreateLineNode::BlockGridInputPinName = TEXT("Block Grid");
const FName UCadenceGridCreateLineNode::PointAInputPinName = TEXT("Point A");
const FName UCadenceGridCreateLineNode::PointBInputPinName = TEXT("Point B");

void UCadenceGridCreateLineNode::CreateInputPins()
{
	Super::CreateInputPins();

	AddInputVariablePin(BlockGridInputPinName, UCadenceVariableActor::StaticClass());
	AddInputVariablePin(PointAInputPinName, UCadenceVariableVector2D::StaticClass());
	AddInputVariablePin(PointBInputPinName, UCadenceVariableVector2D::StaticClass());
}

void UCadenceGridCreateLineNode::CreateOutputPins()
{
	Super::CreateOutputPins();

	AddOutputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGridCreateLineNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* BlockGridInputPin = GetInputPin(BlockGridInputPinName);
	
	UCadenceVariableActor* BlockGridVariable = BlockGridInputPin->GetVariable<UCadenceVariableActor>();
	ACadenceBlockGridActor* BlockGridActor = Cast<ACadenceBlockGridActor>(BlockGridVariable->GetValue());
	
	if(!BlockGridActor)
	{
		UE_LOG(LogCadence, Error, TEXT("CreateLineNode: Block Grid not set!"));
		return ECadenceNodeExecuteResult::Failed;
	}

	UCadenceGraphNodePin* PointAInputPin = GetInputPin(PointAInputPinName);
	UCadenceGraphNodePin* PointBInputPin = GetInputPin(PointBInputPinName);
	
	UCadenceVariableVector2D* PointAVariable = PointAInputPin->GetVariable<UCadenceVariableVector2D>();
	UCadenceVariableVector2D* PointBVariable = PointBInputPin->GetVariable<UCadenceVariableVector2D>();
	
	FVector PointAWorldLocation = BlockGridActor->GridPositionToWorldLocation(PointAVariable->GetValue());
	FVector PointBWorldLocation = BlockGridActor->GridPositionToWorldLocation(PointBVariable->GetValue());

	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* OutputVariable = OutputPin->GetVariable<UCadenceVariableActor>();

	UClass* LineMeshClass = InContext->Graph->ActorSettings->GetLineMeshSplineActorType();
	
	ACadenceMeshSplineActor* MeshSplineActor = SpawnActor<ACadenceMeshSplineActor>(LineMeshClass, InContext, OutputPin, PointAWorldLocation);
	MeshSplineActor->SetSplinePoints({PointAWorldLocation, PointBWorldLocation}, ESplineCoordinateSpace::World);	

	OutputVariable->SetValue(MeshSplineActor);
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceGridGetGridNode::CreateOutputPins()
{
	Super::CreateOutputPins();
	AddOutputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGridGetGridNode::Execute(UCadenceContext* InContext)
{
	Lifetime = ECadenceActorLifetime::Persist;

	ACadenceBlockGridActor* BlockGridActor = nullptr;
	for(TActorIterator<ACadenceBlockGridActor> Iter(GetWorld()); Iter; ++Iter)
	{
		if(BlockGridActor != nullptr)
		{
			UE_LOG(LogCadence, Warning, TEXT("Multiple Block Grid Actors found in scene, Get Grid Node will randomly pick one! Use actor references instead!"));
			break;
		}
		
		BlockGridActor = *Iter;	
	}

	if(!BlockGridActor)
	{
		UE_LOG(LogCadence, Error, TEXT("No Block Grid Actors found in scene, Get Grid Node failed!"));
		return ECadenceNodeExecuteResult::Failed;
	}
	
	
	
	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* OutputVariable = OutputPin->GetVariable<UCadenceVariableActor>();

	FGuid TempGUID = OutputVariable->GetGUID();
	UE_LOG(LogCadence, Log, TEXT("GetGridNode Register: %s - %s"), *OutputPin->GetParentNode()->GetName(), *TempGUID.ToString());
	
	InContext->ActorLifetimeManager->RegisterActor(BlockGridActor, Lifetime, OutputPin->GetVariable()->GetGUID());		

	OutputVariable->SetValue(BlockGridActor);
	
	return ECadenceNodeExecuteResult::Complete;
}
