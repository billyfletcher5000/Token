// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphNodePin.h"
#include "CadenceVariable.h"

#include "CadenceGraphNode.generated.h"

class UCadenceGraph;
class UCadenceGraphNodePin;
class UCadenceContext;

UENUM()
enum class ECadenceNodeExecuteResult : uint8
{
	Running = 0,
	Complete,
	AdditionalExecPinActuated,
	Failed
};


/**
 * 
 */
UCLASS(Abstract)
class CADENCE_API UCadenceGraphNode : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void PostLoad() override;
	void GenerateGUID();
	
	virtual FText GetNodeTitle() const { return GetNodeMenuName(); }
	virtual FText GetNodeMenuName() const { return FText::FromString(StaticClass()->GetName()); }
	virtual FText GetNodeShortDisplayName() const { return GetNodeMenuName(); }
	virtual FText GetNodeCategory() const { return FText::FromName(NAME_Default); }
	virtual FText GetCreateNodeTooltip() const { return FText::FromString(TEXT("Create new node"));}
	virtual FLinearColor GetNodeTitleColor() const { return FLinearColor::Blue; }
	
	virtual bool IsPure() const { return false; }
	virtual bool IsReroute() const { return false; }
	virtual bool IsLatent() const { return false; }
	
	virtual bool HasInputExecPin() const { return true; }
	virtual bool ShouldCreateThenExecPin() const { return true; }

	virtual void DestroyNode();
	
	virtual void CreateInputPins();
	virtual void CreateOutputPins();
	virtual void ClearConnections();

	virtual TArray<UCadenceGraphNodePin*> GetActuatingOutputExecPins() const { return { GetThenPin() }; }

	TObjectPtr<UCadenceGraphNodePin> GetExecPin() const;
	TObjectPtr<UCadenceGraphNodePin> GetThenPin() const;

	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) PURE_VIRTUAL(UCadenceGraphNode::Execute, return ECadenceNodeExecuteResult::Complete;);
	
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetInputPins() { return InputPins; }
	TArray<TObjectPtr<UCadenceGraphNodePin>>& GetOutputPins() { return OutputPins; }

	TObjectPtr<UCadenceGraphNodePin> GetInputPin(const FName& InPinName) const;
	TObjectPtr<UCadenceGraphNodePin> GetOutputPin(const FName& InPinName) const;

	UCadenceGraphNodePin* GetMostAppropriateAutomaticInputPin(UCadenceGraphNodePin* OtherPin);
	UCadenceGraphNodePin* GetMostAppropriateAutomaticOutputPin(UCadenceGraphNodePin* OtherPin);
	
	void SetParentGraph(TObjectPtr<UCadenceGraph> InParentGraph) { ParentGraph = InParentGraph; }
	UCadenceGraph* GetParentGraph() const { return ParentGraph; }

	FVector2D GetGraphPosition() const { return GraphPosition; }
	void SetGraphPosition(const FVector2D& InGraphPosition) { GraphPosition.X = InGraphPosition.X; GraphPosition.Y = InGraphPosition.Y; }
	void SetGraphPosition(const double& X, const double& Y) { GraphPosition.X = X; GraphPosition.Y = Y; }

	FGuid GetGUID() const { return GUID; }
	FString GetDebugName() const { return DebugName; }	
	
#if WITH_EDITOR
	virtual bool CanBeAutoCreated() const { return true; }
	virtual bool DoesSupportEditorValuePropagation() const { return false; }

	bool HasAnyPinsWithWildcard(const int32& InWildcardId);
	TArray<UCadenceGraphNodePin*> GetPinsWithWildcard(const int32& InWildcardId);

	int32 GetInputPinIndex(UCadenceGraphNodePin* InPin) const;

	bool IsPendingDeletion() const { return bIsPendingDeletion; }
	void NotifyPendingDeletion() { bIsPendingDeletion = true; }
#endif
	
protected:
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputExecPin(const FName& InPinName, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputExecPin(const FName& InPinName, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariablePin(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariablePinArray(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);	
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariablePinUnique(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariableWildcardPin(const FName& InPinName, const TSet<TSubclassOf<UCadenceVariable>>& InAllowedTypes, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddInputVariableWildcardArrayPin(const FName& InPinName, const TSet<TSubclassOf<UCadenceVariable>>& InAllowedTypes, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePin(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePinArray(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePinUnique(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariableWildcardPin(const FName& InPinName, const int32& InWildcardId = 0, const int32& InIndex = -1);	
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariableWildcardPin(const FName& InPinName, const TSet<TSubclassOf<UCadenceVariable>>& InAllowedTypes, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariableWildcardArrayPin(const FName& InPinName, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> AddOutputVariableWildcardArrayPin(const FName& InPinName, const TSet<TSubclassOf<UCadenceVariable>>& InAllowedTypes, const int32& InWildcardId = 0, const int32& InIndex = -1);
	virtual TObjectPtr<UCadenceGraphNodePin> CreateExecPin(const FName& InPinName);	
	virtual TObjectPtr<UCadenceGraphNodePin> CreateVariablePin(const FName& InPinName, const TSubclassOf<UCadenceVariable>& InVariableClass, const bool InIsArray = false);
	virtual TObjectPtr<UCadenceGraphNodePin> CreateVariableWildcardPin(const FName& InPinName, const int32& InWildcardId, const TSet<TSubclassOf<UCadenceVariable>>& InAllowedTypes, const bool InIsArray = false);
	virtual bool RemoveInputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);
	virtual bool RemoveOutputPin(const TObjectPtr<UCadenceGraphNodePin>& InPin);
	virtual void RemoveAllInputPins();
	virtual void RemoveAllOutputPins();
	virtual void RemoveAllPins();

	template<typename TVar, typename TVal>
	TObjectPtr<UCadenceGraphNodePin> AddInputVariablePinDefault(const FName& InPinName, const TVal& InDefaultValue, const int32& InIndex = -1)
	{
		TObjectPtr<UCadenceGraphNodePin> Pin = AddInputVariablePin(InPinName, TVar::StaticClass(), InIndex);
		TVar* Variable = Pin->GetVariable<TVar>();
		if(Variable->SupportsDefault())
			Variable->SetValue(InDefaultValue);
		return Pin;
	}

	template<typename TVar, typename TVal>
	TObjectPtr<UCadenceGraphNodePin> AddOutputVariablePinDefault(const FName& InPinName, const TVal& InDefaultValue, const int32& InIndex = -1)
	{
		TObjectPtr<UCadenceGraphNodePin> Pin = AddOutputVariablePin(InPinName, TVar::StaticClass(), InIndex);
		TVar* Variable = Pin->GetVariable<TVar>();
		if(Variable->SupportsDefault())
			Variable->SetValue(InDefaultValue);
		return Pin;
	}

	template<typename TVar, typename TVal>
	bool GetInputPinValue(const FName& InPinName, TVal& OutValue)
	{
		UCadenceGraphNodePin* Pin = GetInputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;

		OutValue = Variable->GetValue();		
		return true;
	}

	template<typename TVar, typename TVal>
	bool GetInputPinValueEnum(const FName& InPinName, TVal& OutValue)
	{
		UCadenceGraphNodePin* Pin = GetInputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;

		OutValue = Variable->template GetValue<TVal>();		
		return true;
	}

	template<typename TVar, typename TVal>
	bool GetOutputPinValue(const FName& InPinName, TVal& OutValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;

		OutValue = Variable->GetValue();		
		return true;
	}

	template<typename TVar, typename TVal>
	bool GetOutputPinValueEnum(const FName& InPinName, TVal& OutValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;

		OutValue = Variable->template GetValue<TVal>();		
		return true;
	}

	template<typename TVar, typename TVal>
	bool SetOutputPinValue(const FName& InPinName, TVal InValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;
		
		Variable->SetValue(InValue);		
		return true;
	}

	template<typename TVar, typename TVal>
	bool SetOutputPinValueEnum(const FName& InPinName, TVal InValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		TVar* Variable = Pin->GetVariable<TVar>();
		if(!IsValid(Variable))
			return false;

		Variable->template SetValue<TVar>(InValue);		
		return true;
	}

	template<typename TVar, typename TVal>
	bool SetOutputPinValueArray(const FName& InPinName, TArray<TVal> InValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		UCadenceVariableArray* ArrayVariable = Pin->GetVariable<UCadenceVariableArray>();
		if(!IsValid(ArrayVariable))
			return false;

		TArray<UCadenceVariable*> Values;
		for(TVal& Val : InValue)
		{
			TVar* ItemVariable = NewObject<TVar>(ArrayVariable->GetOuter());
			ItemVariable->SetValue(Val);
			Values.Add(ItemVariable);
		}
		
		ArrayVariable->SetValue(Values, false);
		return true;
	}

	template<typename TVar, typename TVal>
	bool SetOutputPinValueArrayEnum(const FName& InPinName, TArray<TVal> InValue)
	{
		UCadenceGraphNodePin* Pin = GetOutputPin(InPinName);
		if(!IsValid(Pin))
			return false;

		UCadenceVariableArray* ArrayVariable = Pin->GetVariable<TVar>();
		if(!IsValid(ArrayVariable))
			return false;

		TArray<UCadenceVariable*> Values;
		for(TVal& Val : InValue)
		{
			TVar* ItemVariable = NewObject<TVar>(ArrayVariable->GetOuter());
			ItemVariable->template SetValue<TVar>(Val);
			Values.Add(ItemVariable);
		}
		
		ArrayVariable->SetValue(Values, false);
		return true;
	}

private:
	static TObjectPtr<UCadenceGraphNodePin> GetPinFromArray(const TArray<TObjectPtr<UCadenceGraphNodePin>>& InPinArray, const FName& InPinName);

	void AddPinToList(UCadenceGraphNodePin* InPin, TArray<TObjectPtr<UCadenceGraphNodePin>>& InList, const int32& InIndex);
	void RebuildAndValidateWildcardToVariableClass();
	void OnPinConnectedToWildcardPin(UCadenceGraphNodePin* InConnectedPin, UCadenceGraphNodePin* InWildcardPin);
	void OnPinConnectionsClearedFromWildcardPin(UCadenceGraphNodePin* InWildcardPin);
	
public:
	UPROPERTY()
	TObjectPtr<UCadenceGraph> ParentGraph;
	
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> InputPins;

	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> OutputPins;

	UPROPERTY()
	FVector2D GraphPosition;

private:
	UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid GUID;

	UPROPERTY(EditAnywhere)
	FString DebugName;

	TMap<int32, UClass*> WildcardIdToVariableClass;

#if WITH_EDITOR
	bool bIsPendingDeletion = false;
#endif
};
