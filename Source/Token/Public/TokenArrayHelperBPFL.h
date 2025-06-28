

#pragma once

#include "CoreMinimal.h"

#include "TokenArrayHelperBPFL.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FSortByPredicateActor, class AActor*, InActorA, class AActor*, InActorB);

/**
 * 
 */
UCLASS()
class TOKEN_API UTokenArrayHelperBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Array")
	static void SortByPredicate(UPARAM(ref) TArray<AActor*>& InOutArray, const FSortByPredicateActor& InPredicateActor);
};