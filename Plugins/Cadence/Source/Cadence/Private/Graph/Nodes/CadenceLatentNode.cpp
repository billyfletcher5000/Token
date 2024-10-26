// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/Nodes/CadenceLatentNode.h"

#include "CadenceContext.h"
#include "TickableActions/ICadenceTickableAction.h"

ECadenceNodeExecuteResult UCadenceLatentNode::Execute(UCadenceContext* InContext)
{
	FCadencePathwayActionListPair* ActionListPair = nullptr;
	if(!ActionListMap.Contains(InContext->Pathway))
	{
		TArray<TScriptInterface<ICadenceTickableAction>> NewActionList;
		CreateLatentActions(NewActionList, InContext);

		ActionListMap.Add(FCadencePathwayActionListPair::Create(InContext->Pathway, NewActionList));

		for (TScriptInterface<ICadenceTickableAction> CadenceTickableAction : NewActionList)
		{
			CadenceTickableAction->Init();
		}
	}	
	
	ActionListPair = ActionListMap.FindByKey(InContext->Pathway);

	TArray<TScriptInterface<ICadenceTickableAction>> ActionsToRemove;
	
	for(TScriptInterface<ICadenceTickableAction> Action : ActionListPair->ActionList)
	{
		if(Action->Tick(InContext->DeltaSeconds))
		{
			ActionsToRemove.Add(Action);
		}
	}

	ActionListPair->ActionList.RemoveAll([&](const TScriptInterface<ICadenceTickableAction>& Action)
	{ 
		return ActionsToRemove.Contains(Action);
	});

	if(ActionListPair->ActionList.Num() == 0)
	{
		ActionListMap.RemoveSingle(*ActionListPair);
		return ECadenceNodeExecuteResult::Complete;
	}

	return ECadenceNodeExecuteResult::Running;
}
