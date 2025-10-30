// Copyright (C) Billy Fletcher 2025

#pragma once

#include "CoreMinimal.h"

#include "CadenceMath.generated.h"

UENUM(BlueprintType)
namespace ECadenceEasingFunc
{
	enum Type : int
	{
		/** Simple linear interpolation. */
		Linear,

		/** Simple step interpolation. */
		Step,

		/** Sinusoidal in interpolation. */
		SinusoidalIn,

		/** Sinusoidal out interpolation. */
		SinusoidalOut,

		/** Sinusoidal in/out interpolation. */
		SinusoidalInOut,

		/** Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp. */
		EaseIn,

		/** Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp. */
		EaseOut,

		/** Smoothly accelerates and decelerates.  Ease amount controlled by BlendExp. */
		EaseInOut,

		/** Easing in using an exponential */
		ExpoIn,

		/** Easing out using an exponential */
		ExpoOut,

		/** Easing in/out using an exponential method */
		ExpoInOut,

		/** Easing is based on a half circle. */
		CircularIn,

		/** Easing is based on an inverted half circle. */
		CircularOut,

		/** Easing is based on two half circles. */
		CircularInOut,
	};
}

UCLASS()
class UCadenceMath : public UObject
{
	GENERATED_BODY()

public:
	static double Lerp(double A, double B, double V);
	static float Lerp(float A, float B, float V);
	static FVector Lerp(FVector A, FVector B, float V);
	static FVector2D Lerp(FVector2D A, FVector2D B, float V);
	static FRotator Lerp(FRotator A, FRotator B, float V, bool bShortestPath);
	
	/** Easeing  between A and B using a specified easing function */
	static double Ease(double A, double B, double Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, double BlendExp = 2, int32 Steps = 2);
	static double Ease(float A, float B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp = 2, int32 Steps = 2);
	static FVector Ease(FVector A, FVector B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp = 2, int32 Steps = 2);
	static FVector2D Ease(FVector2D A, FVector2D B, float Alpha, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp = 2, int32 Steps = 2);
	static FRotator Ease(FRotator A, FRotator B, float Alpha, bool bShortestPath, TEnumAsByte<ECadenceEasingFunc::Type> EasingFunc, float BlendExp = 2, int32 Steps = 2);
};

FORCEINLINE_DEBUGGABLE
double UCadenceMath::Lerp(double A, double B, double V)
{
	return A + V*(B-A);
}

FORCEINLINE_DEBUGGABLE
float UCadenceMath::Lerp(float A, float B, float V)
{
	return A + V*(B-A);
}

FORCEINLINE_DEBUGGABLE
FVector UCadenceMath::Lerp(FVector A, FVector B, float V)
{
	return A + V*(B-A);
}

FORCEINLINE_DEBUGGABLE
FVector2D UCadenceMath::Lerp(FVector2D A, FVector2D B, float V)
{
	return A + V*(B-A);
}