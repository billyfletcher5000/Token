// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LevelSequence.h"
#include "CadenceGraph.generated.h"

class UCadenceSequencerTrack;
class UCadenceSequencerSection;
class UCadenceVariable;
class UCadenceGraphNode;
class UCadenceActorSettings;

USTRUCT(BlueprintType)
struct FCadenceNamedVariable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceVariable> Variable;
};

USTRUCT(BlueprintType)
struct FCadenceGraphUserVariableSet 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FCadenceNamedVariable> Variables;
};

UCLASS(BlueprintType)
class CADENCE_API UCadenceGraph : public UObject
{
	GENERATED_BODY()

public:
	UCadenceGraph();
	
	TObjectPtr<UCadenceGraphNode> CreateNode(TSubclassOf<UCadenceGraphNode> InClass, const FVector2D& Location);
	
	void AddNode(TObjectPtr<UCadenceGraphNode> InNode);
	void RemoveNode(TObjectPtr<UCadenceGraphNode> InNode);
	void ClearNodes();

	TArray<TObjectPtr<UCadenceGraphNode>> GetNodes() const { return Nodes; }
	TArray<TObjectPtr<UCadenceGraphNode>> GetRootExecNodes() const;

	TArray<TObjectPtr<UCadenceGraphNode>> GetRootExecNodesThatLeadToNode(UCadenceGraphNode* InNode) const;

	template<typename T>
	TArray<T*> GetNodesOfType() const;

	TArray<TObjectPtr<UCadenceVariable>> GetInputVariables() const;
	TArray<TObjectPtr<UCadenceVariable>> GetOutputVariables() const;
	
	ULevelSequence* GetSequence() const { return Sequence; }
	float GetBPM() const { return BPM; }
	FQuartzTimeSignature GetTimeSignature() const { return TimeSignature; }

	TArray<UCadenceSequencerTrack*> GetTracks() const;
	TArray<UCadenceSequencerSection*> GetSections() const;

	UPROPERTY()
	TArray<FName> CategorySorting;

#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE(FOnPinTypesChanged);

	FOnPinTypesChanged OnPinTypesChanged;
	void NotifyPinTypesChanged() const { OnPinTypesChanged.Broadcast(); }
#endif
	
private:
	void GatherRootExecNodes(TArray<TObjectPtr<UCadenceGraphNode>>& InRootNodeList, UCadenceGraphNode* InNode) const;

public:
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNode>> Nodes;

	UPROPERTY(EditAnywhere)
	TObjectPtr<ULevelSequence> Sequence;

	UPROPERTY(EditAnywhere)
	float BPM = 60.0f;
	
	UPROPERTY(EditAnywhere)
	FQuartzTimeSignature TimeSignature;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCadenceActorSettings> ActorSettings;
	
	UPROPERTY(EditAnywhere)
	FCadenceGraphUserVariableSet UserVariables;
};

template <typename T>
TArray<T*> UCadenceGraph::GetNodesOfType() const
{
	TArray<T*> OutArray;
	
	for (UCadenceGraphNode* Node : Nodes)
	{
		if(T* CastedNode = Cast<T>(Node))
			OutArray.Add(CastedNode);
	}

	return OutArray;
}
