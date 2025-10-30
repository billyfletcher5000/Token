// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CadenceAsset.generated.h"

class UCadenceGraph;

UCLASS(BlueprintType)
class CADENCE_API UCadenceAsset : public UObject
{
	GENERATED_BODY()

public:
	void CreateGraph();
	
	UCadenceGraph* GetPrimaryGraph() const { return Graph; }
	void SetGraph(UCadenceGraph* InGraph) { Graph = InGraph; }

public:
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

public:
	DECLARE_MULTICAST_DELEGATE(FOnPreSave)
	FOnPreSave OnPreSaveDelegate;
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraph> Graph;
};
