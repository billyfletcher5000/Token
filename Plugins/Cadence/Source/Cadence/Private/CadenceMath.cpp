// Copyright (C) Billy Fletcher 2025


#include "CadenceMath.h"

/** Interpolate a linear alpha value using an ease mode and function. */
template<typename FloatType UE_REQUIRES(TIsFloatingPoint<FloatType>::Value)>
FloatType EaseAlpha(FloatType InAlpha, uint8 EasingFunc, FloatType BlendExp, int32 Steps)
{
	switch (EasingFunc)
	{
	case ECadenceEasingFunc::Step:					return FMath::InterpStep<FloatType>(0.f, 1.f, InAlpha, Steps);
	case ECadenceEasingFunc::SinusoidalIn:			return FMath::InterpSinIn<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::SinusoidalOut:			return FMath::InterpSinOut<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::SinusoidalInOut:		return FMath::InterpSinInOut<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::EaseIn:				return FMath::InterpEaseIn<FloatType>(0.f, 1.f, InAlpha, BlendExp);
	case ECadenceEasingFunc::EaseOut:				return FMath::InterpEaseOut<FloatType>(0.f, 1.f, InAlpha, BlendExp);
	case ECadenceEasingFunc::EaseInOut:				return FMath::InterpEaseInOut<FloatType>(0.f, 1.f, InAlpha, BlendExp);
	case ECadenceEasingFunc::ExpoIn:				return FMath::InterpExpoIn<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::ExpoOut:				return FMath::InterpExpoOut<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::ExpoInOut:				return FMath::InterpExpoInOut<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::CircularIn:			return FMath::InterpCircularIn<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::CircularOut:			return FMath::InterpCircularOut<FloatType>(0.f, 1.f, InAlpha);
	case ECadenceEasingFunc::CircularInOut:			return FMath::InterpCircularInOut<FloatType>(0.f, 1.f, InAlpha);
	}
	return InAlpha;
}

FRotator UCadenceMath::Lerp(FRotator A, FRotator B, float V, bool bShortestPath)
{
	// if shortest path, we use Quaternion to interpolate instead of using FRotator
	if (bShortestPath)
	{
		FQuat AQuat(A);
		FQuat BQuat(B);

		FQuat Result = FQuat::Slerp(AQuat, BQuat, V);

		return Result.Rotator();
	}

	const FRotator DeltaAngle = B - A;
	return A + V*DeltaAngle;
}

double UCadenceMath::Ease(float A, float B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp, int32 Steps)
{
	return Lerp(A, B, EaseAlpha(Alpha, EasingFunc, BlendExp, Steps));
}

FVector UCadenceMath::Ease(FVector A, FVector B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp, int32 Steps)
{
	return Lerp(A, B, EaseAlpha(Alpha, EasingFunc, BlendExp, Steps));
}

FVector2D UCadenceMath::Ease(FVector2D A, FVector2D B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp, int32 Steps)
{
	return Lerp(A, B, EaseAlpha(Alpha, EasingFunc, BlendExp, Steps));
}

FRotator UCadenceMath::Ease(FRotator A, FRotator B, float Alpha, bool bShortestPath, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp, int32 Steps)
{
	return Lerp(A, B, EaseAlpha(Alpha, EasingFunc, BlendExp, Steps), bShortestPath);
}

double UCadenceMath::Ease(double A, double B, double Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, double BlendExp, int32 Steps)
{
	return Lerp(A, B, EaseAlpha(Alpha, EasingFunc, BlendExp, Steps));
}
