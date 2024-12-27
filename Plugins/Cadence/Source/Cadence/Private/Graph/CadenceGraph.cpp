// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/CadenceGraph.h"

#include "CadenceSettings.h"
#include "MovieScene.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "SequencerTrack/CadenceSequencerTrack.h"
#include "SequencerTrack/CadenceSequencerSection.h"

UCadenceGraph::UCadenceGraph()
{	
	const UCadenceSettings* Settings = GetDefault<UCadenceSettings>();
	check(Settings);

	ActorSettings = Settings->GetDefaultActorSettings();
}

TObjectPtr<UCadenceGraphNode> UCadenceGraph::CreateNode(TSubclassOf<UCadenceGraphNode> InClass,	const FVector2D& Location)
{
	TObjectPtr<UCadenceGraphNode> Node = NewObject<UCadenceGraphNode>(this, InClass);

	Node->Modify();
	
	Node->GenerateGUID();
	Node->SetFlags(RF_Transactional);	
	
	Node->SetParentGraph(this);
	Node->SetGraphPosition(Location);
	
	return Node;
}

void UCadenceGraph::AddNode(TObjectPtr<UCadenceGraphNode> InNode)
{
	Modify();
	
	InNode->Rename(nullptr, this, REN_ForceNoResetLoaders | REN_DontCreateRedirectors);
	Nodes.Add(InNode);
}

void UCadenceGraph::RemoveNode(TObjectPtr<UCadenceGraphNode> InNode)
{
	Modify();
	Nodes.Remove(InNode);
}

void UCadenceGraph::ClearNodes()
{
	Nodes.Empty();
}

TArray<TObjectPtr<UCadenceGraphNode>> UCadenceGraph::GetRootExecNodes() const
{
	TArray<TObjectPtr<UCadenceGraphNode>> Result;
	
	for(auto Iter = Nodes.begin(); Iter != Nodes.end(); ++Iter)
	{
		UCadenceGraphNode* Node = (*Iter);

		if(Node->IsPure())
			continue;
					
		UCadenceGraphNodePin* ExecPin = Node->GetExecPin();

		if(ExecPin == nullptr || ExecPin->GetConnectedPins().Num() == 0)
		{
			Result.Add(Node);
		}		
	}

	return Result;
}

TArray<TObjectPtr<UCadenceGraphNode>> UCadenceGraph::GetRootExecNodesThatLeadToNode(UCadenceGraphNode* InNode) const
{
	TArray<TObjectPtr<UCadenceGraphNode>> RootNodes;

	//if(!ensureMsgf(!InNode->IsPure(), TEXT("Pure node calculation currently unsupported!")))
	if(InNode->IsPure() && !InNode->IsReroute())
	{
		return RootNodes;
	}
	
	GatherRootExecNodes(RootNodes, InNode);

	return RootNodes;
}

TArray<UCadenceSequencerTrack*> UCadenceGraph::GetTracks() const
{
	TArray<UCadenceSequencerTrack*> OutTracks;
	
	TArray<UMovieSceneTrack*> Tracks = Sequence->GetMovieScene()->GetTracks();
	for(UMovieSceneTrack* Track : Tracks)
	{
		if(UCadenceSequencerTrack* CadenceTrack = Cast<UCadenceSequencerTrack>(Track))
		{
			OutTracks.Add(CadenceTrack);
		}
	}

	return OutTracks;
}

TArray<UCadenceSequencerSection*> UCadenceGraph::GetSections() const
{
	TArray<UCadenceSequencerSection*> OutSections;
	
	TArray<UMovieSceneTrack*> Tracks = Sequence->GetMovieScene()->GetTracks();
	for(UMovieSceneTrack* Track : Tracks)
	{
		if(UCadenceSequencerTrack* CadenceTrack = Cast<UCadenceSequencerTrack>(Track))
		{
			TArray<UMovieSceneSection*> Sections = CadenceTrack->GetAllSections();
			
			for(UMovieSceneSection* Section : Sections)
			{
				if(UCadenceSequencerSection* CadenceSection = Cast<UCadenceSequencerSection>(Section))
				{
					OutSections.Add(CadenceSection);
				}
			}
		}
	}

	return OutSections;
}

void UCadenceGraph::GatherRootExecNodes(TArray<TObjectPtr<UCadenceGraphNode>>& InRootNodeList, UCadenceGraphNode* InNode) const
{	
	UCadenceGraphNodePin* ExecPin = InNode->GetExecPin();
	if(ExecPin == nullptr)
	{
		InRootNodeList.Add(InNode);
		return;
	}
	
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins = ExecPin->GetConnectedPins();
	if(ConnectedPins.Num() == 0)
	{
		InRootNodeList.Add(InNode);
		return;
	}
	
	for(UCadenceGraphNodePin* ConnectedPin : ConnectedPins)
	{
		GatherRootExecNodes(InRootNodeList, ConnectedPin->GetParentNode());
	}
}
