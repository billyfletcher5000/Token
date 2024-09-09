// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceMath.h"
#include "TickableActions/ICadenceTickableAction.h"

#include "CadenceInterpolator.generated.h"

template<typename T, typename FloatType = float>
class TCadenceInterpolator : ICadenceTickableAction
{
public:	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		Progress += InDeltaSeconds / Duration;
		if(Progress < 1.0)
		{
			UCadenceMath::Ease(Start, End, Progress, Ease);
			return false;
		}
		return true;
	}

protected:
	template<typename TInterp>
	static TInterp* CreateInternal(const T& Start, const T& End, const FloatType& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		TInterp* Val = NewObject<TInterp>();
		Val->Start = Start;
		Val->End = End;
		Val->Duration = Duration;
		Val->Ease = Ease;
		return Val;
	}
	
protected:
	T Start;
	T End;
	FloatType Duration = 0.0f;
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;
	FloatType Progress = 0.0f;
};

template<>
class TCadenceInterpolator<FRotator, float> : public ICadenceTickableAction
{
public:	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		Progress += InDeltaSeconds / Duration;
		if(Progress < 1.0)
		{
			UCadenceMath::Ease(Start, End, Progress, bShortestPath, Ease);
			return false;
		}
		return true;
	}
	
protected:
	template<typename TInterp>
	static TInterp* CreateInternal(const FRotator& Start, const FRotator& End, const float& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease, const bool& bShortestPath = true)
	{
		TInterp* Val = NewObject<TInterp>();
		Val->Start = Start;
		Val->End = End;
		Val->Duration = Duration;
		Val->Ease = Ease;
		Val->bShortestPath = bShortestPath;
		return Val;
	}
	
protected:
	FRotator Start;
	FRotator End;
	float Duration = 0.0f;
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;
	float Progress = 0.0f;
	bool bShortestPath = true;
};

UCLASS()
class UCadenceInterpolatorDouble : public UObject, public TCadenceInterpolator<double, double>
{
	GENERATED_BODY()

public:
	static UCadenceInterpolatorDouble* Create(const double& Start, const double& End, const double& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceInterpolatorDouble>(Start, End, Duration, Ease);
	}
};

UCLASS()
class UCadenceInterpolatorFloat : public UObject, public TCadenceInterpolator<float>
{
	GENERATED_BODY()

public:
	static UCadenceInterpolatorFloat* Create(const float& Start, const float& End, const float& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceInterpolatorFloat>(Start, End, Duration, Ease);
	}
};

UCLASS()
class UCadenceInterpolatorVector : public UObject, public TCadenceInterpolator<FVector>
{
	GENERATED_BODY()

public:
	static UCadenceInterpolatorVector* Create(const FVector& Start, const FVector& End, const float& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceInterpolatorVector>(Start, End, Duration, Ease);
	}
};

UCLASS()
class UCadenceInterpolatorVector2 : public UObject, public TCadenceInterpolator<FVector2D>
{
	GENERATED_BODY()

public:
	static UCadenceInterpolatorVector2* Create(const FVector2D& Start, const FVector2D& End, const float& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceInterpolatorVector2>(Start, End, Duration, Ease);
	}
};

UCLASS()
class UCadenceInterpolatorRotator : public UObject, public TCadenceInterpolator<FRotator>
{
	GENERATED_BODY()

public:
	static UCadenceInterpolatorRotator* Create(const FRotator& Start, const FRotator& End, const float& Duration, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease, const bool& bShortestPath)
	{
		return CreateInternal<UCadenceInterpolatorRotator>(Start, End, Duration, Ease, bShortestPath);
	}
};