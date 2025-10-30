// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class FUObjectStringHelper
{
public:
	template<typename T>
	static T* SetFromString(const FString& InStringValue)
	{
		FString ObjectPathLocal = InStringValue;
		ConstructorHelpers::StripObjectClass(ObjectPathLocal);

		// If this is not a full object path it's a relative path so should be saved as a string
		if (FPackageName::IsValidObjectPath(ObjectPathLocal))
		{
			FSoftObjectPath AssetRef = ObjectPathLocal;
			// @todo: why are we resolving here? We should resolve explicitly 
			// during load or not at all
			if(!GCompilingBlueprint)
			{
				return Cast<T>(AssetRef.TryLoad());
			}
			else
			{
				return Cast<T>(AssetRef.ResolveObject());
			}
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	static FString ConvertToValueString(TObjectPtr<T> InValue)
	{
		if(InValue)
			return InValue.GetPath();
		
		return FString();
	}
};