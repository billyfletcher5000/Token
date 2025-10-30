// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "CadenceVariable.h"
#include "UObject/Object.h"
#include "CadenceGraphNodePin.generated.h"

class UCadenceGraphNode;

UCLASS()
class CADENCE_API UCadenceGraphNodePin : public UObject
{
	GENERATED_BODY()
	
public:
	const TArray<TObjectPtr<UCadenceGraphNodePin>>& GetConnectedPins() const { return ConnectedPins; }
	void SetConnectedPins(TArray<TObjectPtr<UCadenceGraphNodePin>> InPins) { ConnectedPins = InPins; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPinConnection, UCadenceGraphNodePin* InPin);
	DECLARE_MULTICAST_DELEGATE(FOnConnectionsCleared);
	
	void ConnectPin(UCadenceGraphNodePin* InPin);	
	void DisconnectPin(UCadenceGraphNodePin* InPin);
	void ClearConnections();

	FOnPinConnection OnPinConnected;
	FOnPinConnection OnPinDisconnected;
	FOnConnectionsCleared OnConnectionsCleared;
	
	void PruneConnections();
	bool HasConnections() const { return ConnectedPins.Num() > 0; }

	void SetParentNode(UCadenceGraphNode* InParent) { ParentNode = InParent; }
	UCadenceGraphNode* GetParentNode() const { return ParentNode; }
	
	void SetPinName(const FName& InPinName) { PinName = InPinName; }
	FName GetPinName() const { return PinName; }

	bool GetShouldHidePinName() const { return bHidePinName; }
	void SetShouldHidePinName(const bool& InValue) { bHidePinName = InValue; }
	
	void GenerateGUID();
	void SetGUID(const FGuid& InGUID) { GUID = InGUID; }
	FGuid GetGUID()
	{
		if(!GUID.IsValid())
			GenerateGUID();
		
		return GUID;
	}

	bool IsExec() const { return bIsExec; }
	void SetIsExec(const bool& InIsExec) { bIsExec = InIsExec; }
	
	TSubclassOf<UCadenceVariable> GetVariableClass() const { return VariableClass; }
	void SetVariableClass(const TSubclassOf<UCadenceVariable>& InVariableClass);
	
	TSubclassOf<UCadenceVariable> GetVariableSecondaryClass() const { return VariableSecondaryClass; }
	void SetVariableSecondaryClass(const TSubclassOf<UCadenceVariable>& InVariableClass);

	template<typename T = UCadenceVariable>
	T* GetVariable(bool AutoCreate = true);

	void SetVariable(UCadenceVariable* InVariable);

	template<typename T = UCadenceVariable>
	T* CreateVariable();

	void OverrideVariable(UCadenceVariable* InVariable) { VariableOverride = InVariable; }
	void ClearOverrideVariable() { VariableOverride = nullptr; }

	bool IsWildcardPin() const { return WildcardId != -1; }
	int32 GetWildcardId() const { return WildcardId; }
	void SetWildcardId(const int32& InWildcardId) { WildcardId = InWildcardId; }

	bool HasRestrictedWildcardAllowedTypes() const { return AllowedWildcardTypes.Num() > 0; }
	TSet<TSubclassOf<UCadenceVariable>> GetAllowedWildcardTypes() const { return AllowedWildcardTypes; }
	void SetAllowedWildcardTypes(const TSet<TSubclassOf<UCadenceVariable>>& InTypes)  { AllowedWildcardTypes = InTypes; }

	bool IsDefaultDisabled() const { return bDisableDefault; }
	void SetDefaultDisabled(const bool& InValue) { bDisableDefault = InValue; }
	
private:
	UPROPERTY()
	TObjectPtr<UCadenceGraphNode> ParentNode;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	bool bHidePinName = false;
	
	UPROPERTY(NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid GUID;

	UPROPERTY()
	bool bIsExec = false;

	UPROPERTY()
	bool bDisableDefault = false;
	
	UPROPERTY()
	TArray<TObjectPtr<UCadenceGraphNodePin>> ConnectedPins;

	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableClass;

	UPROPERTY()
	TSubclassOf<UCadenceVariable> VariableSecondaryClass;

	UPROPERTY()
	TObjectPtr<UCadenceVariable> Variable;

	UPROPERTY()
	int32 WildcardId = -1;

	TWeakObjectPtr<UCadenceVariable> VariableOverride;

	UPROPERTY()
	TSet<TSubclassOf<UCadenceVariable>> AllowedWildcardTypes;
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
	
	if(UCadenceVariableArray* ArrayVariable = Cast<UCadenceVariableArray>(Variable))
		ArrayVariable->SetVariableClass(VariableSecondaryClass);
	
	return Cast<T>(Variable);
}
