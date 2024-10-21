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


#if WITH_EDITOR
enum class ECadenceGetBestValueResult
{
	Undefined = 0,
	Success,
	Failed_NoConnectedPinsToInputs,
	Failed_VariableIsNotTVar,
	Failed_NodeDoesNotSupportPropagation,
	Failed_MultipleConnectedPins,
	Failed_UnconnectedPinWithoutAutoCreateVariable,
	Failed_UnconnectedPinWithoutOptionalValue
};

class CadenceGridNodeUtility
{
public:
	template<typename TVal, typename TVar>
	static ECadenceGetBestValueResult GetBestValue(UCadenceGraphNodePin* InPin, TVal& OutResult)
	{
		auto ConnectedPins = InPin->GetConnectedPins();
		if(ConnectedPins.Num() == 0)
		{
			UCadenceVariable* Var = InPin->GetVariable();
			if(Var == nullptr)
				return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutAutoCreateVariable;					
				
			if(!Var->SupportsDefault())
				return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutOptionalValue;

			TVar* CastedVar = Cast<TVar>(Var);
			if(Var == nullptr)
				return ECadenceGetBestValueResult::Failed_VariableIsNotTVar;

			OutResult = CastedVar->GetOptionalValue();
			return ECadenceGetBestValueResult::Success;
		}

		TArray<UCadenceGraphNode> NodesToProcess;

		// First check that all pins can actually be used
		for(UCadenceGraphNodePin* OutputPin : ConnectedPins)
		{
			UCadenceGraphNode* Node = OutputPin->GetParentNode();
			ECadenceGetBestValueResult NodeResult = IsNodeAppropriateForPropagation(Node);
			if(NodeResult != ECadenceGetBestValueResult::Success)
			{
				return NodeResult;
			}
		}

		// Then process them
		for(UCadenceGraphNodePin* OutputPin : ConnectedPins)
		{
			UCadenceGraphNode* Node = OutputPin->GetParentNode();
			ECadenceGetBestValueResult NodeResult = IsNodeAppropriateForPropagation(Node);
			if(NodeResult != ECadenceGetBestValueResult::Success)
			{
				return NodeResult;
			}
		}
		
		return ECadenceGetBestValueResult::Success;
	}

private:
	
	template<typename TVal, typename TVar>
	static ECadenceGetBestValueResult GetBestValueRecursive(UCadenceGraphNodePin* InPin, TVal& OutResult, bool InCheckForNodePropagationSupport = true)
	{
		auto ConnectedPins = InPin->GetConnectedPins();
		if(ConnectedPins.Num() == 0)
		{
			UCadenceVariable* Var = InPin->GetVariable();
			if(Var == nullptr)
				return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutAutoCreateVariable;					
				
			if(!Var->SupportsDefault())
				return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutOptionalValue;

			TVar* CastedVar = Cast<TVar>(Var);
			if(Var == nullptr)
				return ECadenceGetBestValueResult::Failed_VariableIsNotTVar;

			OutResult = CastedVar->GetOptionalValue();
			return ECadenceGetBestValueResult::Success;
		}

		TArray<UCadenceGraphNode> NodesToProcess;
		
		for(UCadenceGraphNodePin* OutputPin : ConnectedPins)
		{
			UCadenceGraphNode* Node = OutputPin->GetParentNode();
			ECadenceGetBestValueResult NodeResult = IsNodeAppropriateForPropagation(Node);
			if(NodeResult != ECadenceGetBestValueResult::Success)
			{
				return NodeResult;
			}
		}
		
		return ECadenceGetBestValueResult::Success;
	}

	static ECadenceGetBestValueResult IsNodeAppropriateForPropagation(UCadenceGraphNode* InNode, bool InCheckForNodePropagationSupport = true)
	{
		if(InCheckForNodePropagationSupport && !InNode->DoesSupportEditorValuePropagation())
			return ECadenceGetBestValueResult::Failed_NodeDoesNotSupportPropagation;

		auto InputPins = InNode->GetInputPins();
		for(auto InputPin : InputPins)
		{
			auto ConnectedPins = InputPin->GetConnectedPins();
			int32 NumPins = ConnectedPins.Num();
			if(NumPins > 1)
				return ECadenceGetBestValueResult::Failed_MultipleConnectedPins;

			if(NumPins == 0)
			{
				UCadenceVariable* Var = InputPin->GetVariable();
				if(Var == nullptr)
					return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutAutoCreateVariable;					
				
				if(!Var->SupportsDefault())
					return ECadenceGetBestValueResult::Failed_UnconnectedPinWithoutOptionalValue;				
			}
			else
			{
				ECadenceGetBestValueResult NextNodeResult = IsNodeAppropriateForPropagation(ConnectedPins[0]->GetParentNode());
				if(NextNodeResult != ECadenceGetBestValueResult::Success)
					return NextNodeResult;
			}
		}

		return ECadenceGetBestValueResult::Success;
	}
};

#endif

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

	FVector2D PointAPosition = O_PointA;
	FVector2D PointBPosition = O_PointB;

	if(PointAInputPin->HasConnections())
	{
		UCadenceVariableVector2D* PointAVariable = PointAInputPin->GetVariable<UCadenceVariableVector2D>();
		PointAPosition = PointAVariable->GetValue();
	}

	if(PointBInputPin->HasConnections())
	{
		UCadenceVariableVector2D* PointBVariable = PointBInputPin->GetVariable<UCadenceVariableVector2D>();
		PointBPosition = PointBVariable->GetValue();
	}
	
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

	// TODO: Traverse pins at edit time to get these values from input pins if connected
	LineCommand->PositionStart = O_PointA;
	LineCommand->PositionEnd = O_PointB;
	LineCommand->Thickness = LineWidth;
	LineCommand->Color = FLinearColor::Blue;

	InDrawCommandList.Add(LineCommand);

	UCadenceGridPreviewDrawPointCommand* PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = O_PointA;
	PointCommand->Color = FLinearColor::Red;

	InDrawCommandList.Add(PointCommand);

	PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = O_PointB;
	PointCommand->Color = FLinearColor::Green;
	
	InDrawCommandList.Add(PointCommand);

	FVector2D PivotPosition;
	switch (PivotType)
	{
		case ECadenceSplinePivot::Manual:
		case ECadenceSplinePivot::CentreOfPoints:
			PivotPosition = FMath::Lerp(O_PointA, O_PointB, 0.5f);
			break;

		case ECadenceSplinePivot::SpecificPoint:
			PivotPosition = PivotSpecificPointIndex == 1 ? O_PointB : O_PointA;
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

#if WITH_EDITOR
void UCadenceGridMoveToPointNode::GetPreviewDrawCommands(TArray<UCadenceGridPreviewDrawCommand*>& InDrawCommandList)
{
	UCadenceGridPreviewDrawPointCommand* PointCommand = NewObject<UCadenceGridPreviewDrawPointCommand>(GetTransientPackage());
	PointCommand->Position = O_Position;
	PointCommand->Color = FLinearColor::Red;

	InDrawCommandList.Add(PointCommand);
}
#endif

