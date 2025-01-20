// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/CadenceGraphAddPinInterface.h"
#include "Graph/CadenceGraphNode.h"
#include "UObject/Object.h"
#include "CadenceOperationNodes.generated.h"

UCLASS(Abstract)
class CADENCE_API UCadenceOperation : public UObject
{
	GENERATED_BODY()
	
public:	
	virtual bool ApplyOperation(UCadenceVariable* InVariableA, TArray<UCadenceVariable*> InVariableBs, UCadenceVariable* OutResultVariable) PURE_VIRTUAL(UCadenceArithmeticOperation::ApplyOperation, return false;);

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceVariable> GetPrimaryType() const PURE_VIRTUAL(UCadenceArithmeticOperation::GetPrimaryType, return nullptr;);
	virtual TSubclassOf<UCadenceVariable> GetSecondaryType() const { return GetPrimaryType(); }
	virtual TSubclassOf<UCadenceVariable> GetResultType() const { return GetPrimaryType(); }
	virtual bool SupportsAdditionalSecondary() const { return false; }
#endif
};

UCLASS(Abstract)
class CADENCE_API UCadenceOperationNode_Base : public UCadenceGraphNode, public ICadenceGraphAddPinInterface
{
	GENERATED_BODY()
public:	
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual ECadenceNodeExecuteResult Execute(UCadenceContext* InContext) override;

	// ICadenceGraphAddPinInterface
	virtual UCadenceGraphNodePin* AddUserInputPin() override;
	virtual bool RemoveUserInputPin(UCadenceGraphNodePin* Pin) override;
	virtual bool CanRemovePin(const UCadenceGraphNodePin* Pin) const override;
	// ~ICadenceGraphAddPinInterface

	virtual bool IsPure() const override { return true; }	

#if WITH_EDITOR
	virtual TSubclassOf<UCadenceOperation> GetOperationBase() const { return UCadenceOperation::StaticClass(); }
	bool SetOperation(UCadenceOperation* InOperation, const bool& bInIsReversed);

	bool IsPinPrimary(UCadenceGraphNodePin* InPin) const { return PrimaryInputPin == InPin; }
	bool IsPinSecondary(UCadenceGraphNodePin* InPin) const { return SecondaryInputPins.Contains(InPin); }
	bool IsPinResult(UCadenceGraphNodePin* InPin) const { return ResultOutputPin == InPin; }

	UClass* GetPrimaryVariableType(const bool& InRequireConnection = false) const;
	UClass* GetSecondaryVariableType(const bool& InRequireConnection = false) const;
	UClass* GetResultVariableType(const bool& InRequireConnection = false) const;

	bool HasMultipleSecondaryPins() const { return SecondaryInputPins.Num() > 1; }
#endif

#if WITH_EDITORONLY_DATA
	void SetPrimaryAllowedTypes(const TSet<TSubclassOf<UCadenceVariable>>& InTypes) { PrimaryAllowedTypes = InTypes; }
	TSet<TSubclassOf<UCadenceVariable>>& GetPrimaryAllowedTypes() { return PrimaryAllowedTypes; }
	bool IsPrimaryTypeAllowed(UClass* InType) const { return PrimaryAllowedTypes.IsEmpty() || PrimaryAllowedTypes.Contains(InType); }
	void SetSecondaryAllowedTypes(const TSet<TSubclassOf<UCadenceVariable>>& InTypes) { SecondaryAllowedTypes = InTypes; }
	TSet<TSubclassOf<UCadenceVariable>>& GetSecondaryAllowedTypes() { return SecondaryAllowedTypes; }
	bool IsSecondaryTypeAllowed(UClass* InType) const { return SecondaryAllowedTypes.IsEmpty() || SecondaryAllowedTypes.Contains(InType); }
	void SetResultAllowedTypes(const TSet<TSubclassOf<UCadenceVariable>>& InTypes) { ResultAllowedTypes = InTypes; }
	TSet<TSubclassOf<UCadenceVariable>>& GetResultAllowedTypes() { return ResultAllowedTypes; }
	bool IsResultTypeAllowed(UClass* InType) const { return ResultAllowedTypes.IsEmpty() || ResultAllowedTypes.Contains(InType); }
#endif

protected:
	bool RefreshPins();
	virtual bool RefreshInputPins();
	virtual bool RefreshOutputPins();
	
private:
	TObjectPtr<UCadenceGraphNodePin> AddSecondaryInputPin();
	UClass* GetPrimaryVariableClass() const;
	UClass* GetSecondaryVariableClass() const;
	bool HasAnyConnections() const;

private:
	UPROPERTY()
	TObjectPtr<UCadenceOperation> Operation;

	UPROPERTY()
	bool bIsOperationReversed;
	
	UPROPERTY()
	int32 PinIndex = 0;

	UPROPERTY()
	TWeakObjectPtr<UCadenceGraphNodePin> PrimaryInputPin;

	UPROPERTY()
	TArray<TWeakObjectPtr<UCadenceGraphNodePin>> SecondaryInputPins;

	UPROPERTY()
	TWeakObjectPtr<UCadenceGraphNodePin> ResultOutputPin;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TSet<TSubclassOf<UCadenceVariable>> PrimaryAllowedTypes;

	UPROPERTY()
	TSet<TSubclassOf<UCadenceVariable>> SecondaryAllowedTypes;
	
	UPROPERTY()
	TSet<TSubclassOf<UCadenceVariable>> ResultAllowedTypes;
#endif

	static const int32 PrimaryWildcardId;
	static const int32 SecondaryWildcardId;
	static const int32 ResultWildcardId;
};
