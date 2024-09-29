// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceRerouteNodes.generated.h"

class UCadenceVariable;

UCLASS()
class CADENCE_API UCadenceSimpleRerouteNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return !bIsExecReroute; }
	virtual bool IsReroute() const override { return true; }
	
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext);

	virtual void SetAsExecReroute();
	virtual void SetVariableType(const TSubclassOf<UCadenceVariable>& InVariableType);
	void Clear();
	void CheckRerouteTypeValid();
	virtual TSubclassOf<UCadenceVariable> GetVariableType() const { return VariableType; }

	UCadenceGraphNodePin* GetRerouteInputPin() const;
	UCadenceGraphNodePin* GetRerouteOutputPin() const;

protected:
	UPROPERTY()
	bool bIsExecReroute = false;
	
	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableType = nullptr;
};
