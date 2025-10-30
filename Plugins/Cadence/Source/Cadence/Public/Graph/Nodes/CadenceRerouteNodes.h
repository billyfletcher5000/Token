// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "Graph/CadenceGraphNodePin.h"
#include "UObject/Object.h"
#include "CadenceRerouteNodes.generated.h"

class UCadenceVariable;

UCLASS(Abstract)
class CADENCE_API UCadenceRerouteNodeBase : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return !bIsExecReroute; }
	virtual bool IsReroute() const override { return true; }

	// Reroute nodes should never execute
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override final { return ECadenceNodeExecuteResult::Failed; }

	virtual void SetAsExecReroute();
	virtual void SetVariableType(const TSubclassOf<UCadenceVariable>& InVariableType);
	virtual void Clear();
	virtual void CheckRerouteTypeValid();

	virtual TSubclassOf<UCadenceVariable> GetVariableType() const { return VariableType; }

	virtual UCadenceGraphNodePin* GetRerouteInputPin() const;
	virtual UCadenceGraphNodePin* GetRerouteOutputPin() const;
	UCadenceGraphNode* GetRerouteInputNode() const;
	TArray<UCadenceGraphNodePin*> GetRerouteOutputNodeConnectedInputPins() const;

private:	
	void GetRerouteOutputNodeConnectedInputPins(TArray<UCadenceGraphNodePin*>& InResult) const;
	
protected:
	UPROPERTY()
	bool bIsExecReroute = false;
	
	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableType = nullptr;
};

UCLASS()
class CADENCE_API UCadenceSimpleRerouteNode : public UCadenceRerouteNodeBase
{	
	GENERATED_BODY()
};