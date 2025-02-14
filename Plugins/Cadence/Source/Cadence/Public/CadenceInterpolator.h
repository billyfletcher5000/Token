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
	T GetValue() const { return Value; }
	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		Progress += InDeltaSeconds / Duration;
		if(Progress < 1.0)
		{
			Value = UCadenceMath::Ease(Start, End, Progress, Ease);
			return false;
		}

		Value = End;
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

	T Value;
};

template<typename T, typename FloatType = float>
class TCadenceSteppedInterpolator : ICadenceTickableAction
{
public:
	T GetValue() const { return Value; }
	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		ProgressSeconds += InDeltaSeconds;
		if(ProgressSeconds < TotalDuration)
		{
			FloatType CombinedStepSize = StepTransitionDuration + StepDelay;
			int32 Step = FMath::FloorToInt32(ProgressSeconds / CombinedStepSize);
			FloatType CombinedStepProgress = FMath::Fmod(ProgressSeconds, CombinedStepSize);
			if(CombinedStepProgress < StepTransitionDuration)
			{
				FloatType StepTransitionProgress = CombinedStepProgress / StepTransitionDuration;
				FloatType TotalSteps = FMath::CeilToFloat(TotalDuration / CombinedStepSize);
				T Diff = End - Start;
				T StepDistance = Diff / TotalSteps;
				T PreviousStep = Start + (StepDistance * Step);
				T NextStep = Start + (StepDistance * (Step + 1));
				
				Value = UCadenceMath::Ease(PreviousStep, NextStep, StepTransitionProgress, Ease);
			}
			// else // We are in a delay and don't need to move					
			
			return false;
		}

		Value = End;
		return true;
	}

protected:
	template<typename TInterp>
	static TInterp* CreateInternal(const T& Start, const T& End, const FloatType& TotalDuration, const FloatType& StepTransitionDuration, const FloatType& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		TInterp* Val = NewObject<TInterp>();
		Val->Start = Start;
		Val->End = End;
		Val->TotalDuration = TotalDuration;
		Val->StepTransitionDuration = StepTransitionDuration;
		Val->StepDelay = StepDelay;
		Val->Ease = Ease;
		return Val;
	}
	
protected:
	T Start;
	T End;
	FloatType TotalDuration = 0.0f;
	FloatType StepTransitionDuration = 0.0f;
	FloatType StepDelay = 0.0f;
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;
	FloatType ProgressSeconds = 0.0f;

	T Value;
};


template<>
class TCadenceInterpolator<FRotator, float> : public ICadenceTickableAction
{
public:	
	FRotator GetValue() const { return Value; }
	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		Progress += InDeltaSeconds / Duration;
		if(Progress < 1.0)
		{
			Value = UCadenceMath::Ease(Start, End, Progress, bShortestPath, Ease);
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
	FRotator Start = FRotator::ZeroRotator;
	FRotator End = FRotator::ZeroRotator;
	float Duration = 0.0f;
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;
	float Progress = 0.0f;
	bool bShortestPath = true;

	FRotator Value = FRotator::ZeroRotator;
};

template<>
class TCadenceSteppedInterpolator<FRotator, float> : ICadenceTickableAction
{
public:
	FRotator GetValue() const { return Value; }
	
	virtual bool Tick(const float& InDeltaSeconds) override
	{
		ProgressSeconds += InDeltaSeconds;
		if(ProgressSeconds < TotalDuration)
		{
			float CombinedStepSize = StepTransitionDuration + StepDelay;
			int32 Step = FMath::FloorToInt32(ProgressSeconds / CombinedStepSize);
			float CombinedStepProgress = FMath::Fmod(ProgressSeconds, CombinedStepSize);
			if(CombinedStepProgress < StepTransitionDuration)
			{
				float StepTransitionProgress = CombinedStepProgress / StepTransitionDuration;
				float TotalSteps = FMath::CeilToFloat(TotalDuration / CombinedStepSize);
				FRotator Diff = End - Start;
				FRotator StepDistance = Diff * (1.0f / TotalSteps);
				FRotator PreviousStep = Start + (StepDistance * Step);
				FRotator NextStep = Start + (StepDistance * (Step + 1));
				
				Value = UCadenceMath::Ease(PreviousStep, NextStep, StepTransitionProgress, bShortestPath, Ease);
			}
			// else // We are in a delay and don't need to move					
			
			return false;
		}

		Value = End;
		return true;
	}

protected:
	template<typename TInterp>
	static TInterp* CreateInternal(const FRotator& Start, const FRotator& End, const float& TotalDuration, const float& StepTransitionDuration, const float& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease, const bool& bShortestPath)
	{
		TInterp* Val = NewObject<TInterp>();
		Val->Start = Start;
		Val->End = End;
		Val->TotalDuration = TotalDuration;
		Val->StepTransitionDuration = StepTransitionDuration;
		Val->StepDelay = StepDelay;
		Val->Ease = Ease;
		return Val;
	}
	
protected:
	FRotator Start = FRotator::ZeroRotator;
	FRotator End = FRotator::ZeroRotator;
	float TotalDuration = 0.0f;
	float StepTransitionDuration = 0.0f;
	float StepDelay = 0.0f;
	TEnumAsByte<ECadenceEasingFunc::Type> Ease = ECadenceEasingFunc::Linear;
	float ProgressSeconds = 0.0f;
	bool bShortestPath = true;

	FRotator Value = FRotator::ZeroRotator;
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

UCLASS()
class UCadenceSteppedInterpolatorDouble : public UObject, public TCadenceSteppedInterpolator<double, double>
{
	GENERATED_BODY()

public:
	static UCadenceSteppedInterpolatorDouble* Create(const double& Start, const double& End, const double& TotalDuration, const double& StepTransitionDuration, const double& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceSteppedInterpolatorDouble>(Start, End, TotalDuration, StepTransitionDuration, StepDelay, Ease);
	}
};

UCLASS()
class UCadenceSteppedInterpolatorFloat : public UObject, public TCadenceSteppedInterpolator<float>
{
	GENERATED_BODY()

public:
	static UCadenceSteppedInterpolatorFloat* Create(const float& Start, const float& End, const float& TotalDuration, const float& StepTransitionDuration, const float& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceSteppedInterpolatorFloat>(Start, End, TotalDuration, StepTransitionDuration, StepDelay, Ease);
	}
};

UCLASS()
class UCadenceSteppedInterpolatorVector : public UObject, public TCadenceSteppedInterpolator<FVector>
{
	GENERATED_BODY()

public:
	static UCadenceSteppedInterpolatorVector* Create(const FVector& Start, const FVector& End, const float& TotalDuration, const float& StepTransitionDuration, const float& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceSteppedInterpolatorVector>(Start, End, TotalDuration, StepTransitionDuration, StepDelay, Ease);
	}
};

UCLASS()
class UCadenceSteppedInterpolatorVector2 : public UObject, public TCadenceSteppedInterpolator<FVector2D>
{
	GENERATED_BODY()

public:
	static UCadenceSteppedInterpolatorVector2* Create(const FVector2D& Start, const FVector2D& End, const float& TotalDuration, const float& StepTransitionDuration, const float& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease)
	{
		return CreateInternal<UCadenceSteppedInterpolatorVector2>(Start, End, TotalDuration, StepTransitionDuration, StepDelay, Ease);
	}
};

UCLASS()
class UCadenceSteppedInterpolatorRotator : public UObject, public TCadenceSteppedInterpolator<FRotator>
{
	GENERATED_BODY()

public:
	static UCadenceSteppedInterpolatorRotator* Create(const FRotator& Start, const FRotator& End, const float& TotalDuration, const float& StepTransitionDuration, const float& StepDelay, const TEnumAsByte<ECadenceEasingFunc::Type>& Ease, const bool& bShortestPath)
	{
		return CreateInternal<UCadenceSteppedInterpolatorRotator>(Start, End, TotalDuration, StepTransitionDuration, StepDelay, Ease, bShortestPath);
	}
};