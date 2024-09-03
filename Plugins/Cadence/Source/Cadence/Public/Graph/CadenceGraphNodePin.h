// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceVariable.h"
#include "UObject/Object.h"
#include "CadenceGraphNodePin.generated.h"

class UCadenceGraphNode;

/**
 * 
 */
UCLASS()
class CADENCE_API UCadenceGraphNodePin : public UObject
{
	GENERATED_BODY()
	
public:
	const TArray<TObjectPtr<UCadenceGraphNodePin>>& GetConnectedPins() const { return ConnectedPins; }
	void SetConnectedPins(TArray<TObjectPtr<UCadenceGraphNodePin>> InPins) { ConnectedPins = InPins; }
	
	void ConnectPin(UCadenceGraphNodePin* InPin);	
	void DisconnectPin(UCadenceGraphNodePin* InPin);
	void ClearConnections();
	void PruneConnections();

	void SetParentNode(UCadenceGraphNode* InParent) { ParentNode = InParent; }
	UCadenceGraphNode* GetParentNode() const { return ParentNode; }
	
	void SetPinName(const FName& InPinName) { PinName = InPinName; }
	FName GetPinName() const { return PinName; }
	
	void GenerateGUID();
	void SetGUID(const FGuid& InGUID) { GUID = InGUID; }
	FGuid GetGUID() const { return GUID; }

	bool IsExec() const { return bIsExec; }
	void SetIsExec(const bool& InIsExec) { bIsExec = InIsExec; }
	
	TSubclassOf<UCadenceVariable> GetVariableClass() const { return VariableClass; }
	void SetVariableClass(TSubclassOf<UCadenceVariable> InVariableClass) { VariableClass = InVariableClass; }

	template<typename T = UCadenceVariable>
	T* GetVariable(bool AutoCreate = true);

	template<typename T = UCadenceVariable>
	T* CreateVariable();

	void OverrideVariable(UCadenceVariable* InVariable) { VariableOverride = InVariable; }
	void ClearOverrideVariable() { VariableOverride = nullptr; }
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> ParentNode;

	UPROPERTY()
	FName PinName;
	
	UPROPERTY()
	FGuid GUID;

	UPROPERTY()
	bool bIsExec = false;
	
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins;

	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableClass;

	UPROPERTY()
	TObjectPtr<UCadenceVariable> Variable;

	UCadenceVariable* VariableOverride;
};

template <typename T>
T* UCadenceGraphNodePin::GetVariable(bool AutoCreate)
{
	if(VariableOverride != nullptr)
		return Cast<T>(VariableOverride);
	
	if(Variable == nullptr && AutoCreate)
		return CreateVariable<T>();

	return Cast<T>(Variable);
}

template <typename T>
T* UCadenceGraphNodePin::CreateVariable()
{	
	Variable = NewObject<UCadenceVariable>(this, VariableClass);
	return Cast<T>(Variable);
}
