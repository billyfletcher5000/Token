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
#include "Graph/Nodes/GridPreview/CadenceGridPreviewDrawCommand.h"
#include "TickableActions/CadenceMoveTickableActions.h"


void UCadenceGridCreateLineNode::CreateInputPins()
{
	Super::CreateInputPins();

	AddInputVariablePin(FCadenceGridNodeConstants::BlockGridInputPinName, UCadenceVariableActor::StaticClass());
	AddInputVariablePinDefault<UCadenceVariableVector2D, FVector2D>(FCadenceGridNodeConstants::PointAInputPinName, FVector2D(0.f, 0.f));
	AddInputVariablePinDefault<UCadenceVariableVector2D, FVector2D>(FCadenceGridNodeConstants::PointBInputPinName, FVector2D(1.f, 1.f));
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
	
	FVector2D PointAPosition = PointAVariable->GetValue();
	FVector2D PointBPosition = PointBVariable->GetValue();
	
	FVector PointAWorldLocation = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(PointAPosition): BlockGridActor->GridPositionToWorldLocation(PointAPosition);
	FVector PointBWorldLocation = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(PointBPosition): BlockGridActor->GridPositionToWorldLocation(PointBPosition);

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

#if WITH_EDITOR
void UCadenceGridCreateLineNode::GetPreviewDrawCommands(TArray<UCadenceGridPreviewDrawCommand*>& InDrawCommandList)
{	
	UCadenceGridPreviewDrawLineCommand* LineCommand = NewObject<UCadenceGridPreviewDrawLineCommand>(GetTransientPackage());

	UCadenceGraphNodePin* PointAInputPin = GetInputPin(FCadenceGridNodeConstants::PointAInputPinName);
	UCadenceGraphNodePin* PointBInputPin = GetInputPin(FCadenceGridNodeConstants::PointBInputPinName);
	UCadenceVariableVector2D* PointAVariable = PointAInputPin->GetVariable<UCadenceVariableVector2D>();
	UCadenceVariableVector2D* PointBVariable = PointBInputPin->GetVariable<UCadenceVariableVector2D>();
	FVector2D PointAPosition = PointAVariable->GetValue();
	FVector2D PointBPosition = PointBVariable->GetValue();
	
	// TODO: Traverse pins at edit time to get these values from input pins if connected, see GetBestValue stuff above
	LineCommand->PositionStart = PointAPosition;
	LineCommand->PositionEnd = PointBPosition;
	LineCommand->Thickness = LineWidth;
	LineCommand->Color = FLinearColor::Blue;

	InDrawCommandList.Add(LineCommand);

	UCadenceGridPreviewDrawPointCommand* PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = PointAPosition;
	PointCommand->Color = FLinearColor::Red;

	InDrawCommandList.Add(PointCommand);

	PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = PointBPosition;
	PointCommand->Color = FLinearColor::Green;
	
	InDrawCommandList.Add(PointCommand);

	FVector2D PivotPosition;
	switch (PivotType)
	{
		case ECadenceSplinePivot::Manual:
		case ECadenceSplinePivot::CentreOfPoints:
			PivotPosition = FMath::Lerp(PointAPosition, PointBPosition, 0.5f);
			break;

		case ECadenceSplinePivot::SpecificPoint:
			PivotPosition = PivotSpecificPointIndex == 1 ? PointBPosition : PointAPosition;
			break;
	}
	
	PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = PivotPosition;
	PointCommand->Color = FLinearColor::Yellow;	

	InDrawCommandList.Add(PointCommand);
}
#endif

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
	AddInputVariablePin(FCadencePinConstants::Pin_Depth, UCadenceVariableFloat::StaticClass());
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
	FVector2D TargetPosition = PositionPin->GetVariable<UCadenceVariableVector2D>()->GetValue();	

	FVector TargetWorldPosition = bUseNormalisedPositions ? BlockGridActor->NormalisedPositionToWorldLocation(TargetPosition) : BlockGridActor->GridPositionToWorldLocation(TargetPosition);

	UCadenceGraphNodePin* DurationPin = GetInputPin(FCadencePinConstants::Pin_Duration);
	float TargetDuration = DurationPin->GetVariable<UCadenceVariableFloat>()->GetValue();	

	InActionList.Add(UCadenceActorTranslateTickable::Create(Actor, TargetDuration, TargetWorldPosition, Easing));
}

#if WITH_EDITOR
void UCadenceGridMoveToPointNode::GetPreviewDrawCommands(TArray<UCadenceGridPreviewDrawCommand*>& InDrawCommandList)
{
	UCadenceGridPreviewDrawPointCommand* PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	
	UCadenceGraphNodePin* PositionPin = GetInputPin(FCadencePinConstants::Pin_Position);
	PointCommand->Position = PositionPin->GetVariable<UCadenceVariableVector2D>()->GetValue();
	PointCommand->Color = FLinearColor::Red;

	InDrawCommandList.Add(PointCommand);
}
#endif

