// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphNode.h"
#include "CadenceArrayNodes.generated.h"

namespace FCadenceArrayConstants
{
	static const FText NodeCategory = FText::FromString(TEXT("Array"));
	static constexpr FLinearColor NodeTitleColor = FLinearColor(1.0f, 0.8f, 0.9f);
}

UCLASS()
class CADENCE_API UCadenceForEachNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual bool ShouldCreateThenExecPin() const override { return false; }
	virtual TArray<UCadenceGraphNodePin*> GetActuatingOutputExecPins() const override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("For Each")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }

private:	
	TWeakObjectPtr<UCadenceGraphNodePin> CurrentActuatingOutputExecPin = nullptr;
	TMap<UCadenceGraphRunnerPathway*, int32> PathwayToIndex;
};

UCLASS()
class CADENCE_API UCadenceArraySizeNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return true; }
	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Size")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceArrayFindIndexNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return true; }
	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Find Index")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceArrayContainsNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual bool IsPure() const override { return true; }
	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Contains")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceArrayAddItemNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Add Item")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceArrayRemoveItemNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Remove Item")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};

UCLASS()
class CADENCE_API UCadenceArrayRemoveByIndexNode : public UCadenceGraphNode
{
	GENERATED_BODY()

public:
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;
	
	virtual FText GetNodeMenuName() const override { return FText::FromString(TEXT("Remove Index")); }
	virtual FText GetNodeCategory() const override { return FCadenceArrayConstants::NodeCategory; }
	virtual FLinearColor GetNodeTitleColor() const override { return FCadenceArrayConstants::NodeTitleColor; }
};