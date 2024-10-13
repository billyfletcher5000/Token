// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceGridNodes.h"

#include "Cadence.h"
#include "EngineUtils.h"
#include "Actors/CadenceActorSettings.h"
#include "Actors/CadenceBlockGridActor.h"
#include "Actors/CadenceBlockGridSplineActor.h"
#include "Actors/CadenceMeshSplineActor.h"
#include "Actors/CadenceSplinePointProxy.h"
#include "Components/SplineComponent.h"
#include "Graph/CadenceGraph.h"
#include "Graph/CadenceGraphNodePin.h"
#include "Graph/CadencePinConstants.h"
#include "Graph/CadenceVariable.h"
#include "TickableActions/CadenceMoveTickableActions.h"


void UCadenceGridCreateLineNode::CreateInputPins()
{
	Super::CreateInputPins();

	AddInputVariablePin(FCadenceGridNodeConstants::BlockGridInputPinName, UCadenceVariableActor::StaticClass());
	AddInputVariablePin(FCadenceGridNodeConstants::PointAInputPinName, UCadenceVariableVector2D::StaticClass());
	AddInputVariablePin(FCadenceGridNodeConstants::PointBInputPinName, UCadenceVariableVector2D::StaticClass());
}

void UCadenceGridCreateLineNode::CreateOutputPins()
{
	Super::CreateOutputPins();

	AddOutputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
	AddOutputVariablePin(FCadenceGridNodeConstants::PointProxyAOutputPinName, UCadenceVariableActor::StaticClass());
	AddOutputVariablePin(FCadenceGridNodeConstants::PointProxyBOutputPinName, UCadenceVariableActor::StaticClass());
}

ECadenceNodeExecuteResult UCadenceGridCreateLineNode::Execute(UCadenceContext* InContext)
{
	UCadenceGraphNodePin* BlockGridInputPin = GetInputPin(FCadenceGridNodeConstants::BlockGridInputPinName);
	
	UCadenceVariableActor* BlockGridVariable = BlockGridInputPin->GetVariable<UCadenceVariableActor>();
	ACadenceBlockGridActor* BlockGridActor = Cast<ACadenceBlockGridActor>(BlockGridVariable->GetValue());
	
	if(!BlockGridActor)
	{
		UE_LOG(LogCadence, Error, TEXT("CreateLineNode: Block Grid not set!"));
		return ECadenceNodeExecuteResult::Failed;
	}

	UCadenceGraphNodePin* PointAInputPin = GetInputPin(FCadenceGridNodeConstants::PointAInputPinName);
	UCadenceGraphNodePin* PointBInputPin = GetInputPin(FCadenceGridNodeConstants::PointBInputPinName);
	
	UCadenceVariableVector2D* PointAVariable = PointAInputPin->GetVariable<UCadenceVariableVector2D>();
	UCadenceVariableVector2D* PointBVariable = PointBInputPin->GetVariable<UCadenceVariableVector2D>();
	
	FVector PointAWorldLocation = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(PointAVariable->GetValue()): BlockGridActor->GridPositionToWorldLocation(PointAVariable->GetValue());
	FVector PointBWorldLocation = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(PointBVariable->GetValue()): BlockGridActor->GridPositionToWorldLocation(PointBVariable->GetValue());

	UCadenceGraphNodePin* OutputPin = GetOutputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* OutputVariable = OutputPin->GetVariable<UCadenceVariableActor>();

	UClass* LineMeshClass = InContext->Graph->ActorSettings->GetLineMeshSplineActorType();
	
	ACadenceMeshSplineActor* MeshSplineActor = SpawnActor<ACadenceMeshSplineActor>(LineMeshClass, InContext, OutputPin, PointAWorldLocation);
	MeshSplineActor->SetSplinePoints({PointAWorldLocation, PointBWorldLocation}, ESplineCoordinateSpace::World);
	MeshSplineActor->SetPivotType(PivotType);
	
	if(PivotType == ECadenceSplinePivot::SpecificPoint)
		MeshSplineActor->SetPivotSpecificPoint(PivotSpecificPointIndex);
	
	if(ACadenceBlockGridSplineActor* BlockGridSplineActor = Cast<ACadenceBlockGridSplineActor>(MeshSplineActor))
	{
		BlockGridSplineActor->SetGridBlockWidth(LineWidth);
	}

	OutputVariable->SetValue(MeshSplineActor);

	UCadenceGraphNodePin* ProxyAOutputPin = GetOutputPin(FCadenceGridNodeConstants::PointProxyAOutputPinName);
	UCadenceGraphNodePin* ProxyBOutputPin = GetOutputPin(FCadenceGridNodeConstants::PointProxyBOutputPinName);

	if(ProxyAOutputPin->HasConnections())
	{
		ACadenceSplinePointProxy* PointProxy = SpawnActor<ACadenceSplinePointProxy>(InContext, ProxyAOutputPin, PointAWorldLocation);
		PointProxy->Init(MeshSplineActor, 0);

		ProxyAOutputPin->GetVariable<UCadenceVariableActor>()->SetValue(PointProxy);
	}

	if(ProxyBOutputPin->HasConnections())
	{
		ACadenceSplinePointProxy* PointProxy = SpawnActor<ACadenceSplinePointProxy>(InContext, ProxyBOutputPin, PointBWorldLocation);
		PointProxy->Init(MeshSplineActor, 1);

		ProxyBOutputPin->GetVariable<UCadenceVariableActor>()->SetValue(PointProxy);
	}
	
	return ECadenceNodeExecuteResult::Complete;
}

void UCadenceGridCreateLineNode::GetPreviewDrawCommands(TArray<FCadenceGridPreviewDrawCommand>& InDrawCommandList)
{
	ICadenceGraphGridCommandProvider::GetPreviewDrawCommands(InDrawCommandList);
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

void UCadenceGridMoveToPointNode::CreateInputPins()
{
	Super::CreateInputPins();
	
	AddInputVariablePin(FCadenceGridNodeConstants::BlockGridInputPinName, UCadenceVariableActor::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_Actor, UCadenceVariableActor::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_Position, UCadenceVariableVector2D::StaticClass());
	AddInputVariablePin(FCadencePinConstants::Pin_Duration, UCadenceVariableFloat::StaticClass());
}

void UCadenceGridMoveToPointNode::CreateLatentActions(TArray<TScriptInterface<ICadenceTickableAction>>& InActionList)
{
	UCadenceGraphNodePin* BlockGridPin = GetInputPin(FCadenceGridNodeConstants::BlockGridInputPinName);
	UCadenceVariableActor* BlockGridVariable = BlockGridPin->GetVariable<UCadenceVariableActor>();
	ACadenceBlockGridActor* BlockGridActor = Cast<ACadenceBlockGridActor>(BlockGridVariable->GetValue());
	
	UCadenceGraphNodePin* ActorPin = GetInputPin(FCadencePinConstants::Pin_Actor);
	UCadenceVariableActor* ActorVariable = ActorPin->GetVariable<UCadenceVariableActor>();
	AActor* Actor = ActorVariable->GetValue();
	
	UCadenceGraphNodePin* PositionPin = GetInputPin(FCadencePinConstants::Pin_Position);
	UCadenceGraphNodePin* DurationPin = GetInputPin(FCadencePinConstants::Pin_Duration);

	FVector2D TargetPosition = O_Position;
	if(PositionPin->HasConnections())
	{
		TargetPosition = PositionPin->GetVariable<UCadenceVariableVector2D>()->GetValue();
	}

	FVector TargetWorldPosition = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(TargetPosition) : BlockGridActor->GridPositionToWorldLocation(TargetPosition);

	float TargetDuration = O_Duration;
	if(DurationPin->HasConnections())
	{
		TargetDuration = DurationPin->GetVariable<UCadenceVariableFloat>()->GetValue();
	}

	InActionList.Add(UCadenceActorTranslateTickable::Create(Actor, TargetDuration, TargetWorldPosition, Easing));
}
