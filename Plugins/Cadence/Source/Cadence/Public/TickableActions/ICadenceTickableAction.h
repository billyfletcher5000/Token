#pragma once

#include "CoreMinimal.h"

#include "ICadenceTickableAction.generated.h"

UINTERFACE()
class CADENCE_API UCadenceTickableAction : public UInterface
{
	GENERATED_BODY()
};

class CADENCE_API ICadenceTickableAction
{
	GENERATED_BODY()
	
public:	
	virtual bool Tick(const float& InDeltaSeconds) = 0;
};
