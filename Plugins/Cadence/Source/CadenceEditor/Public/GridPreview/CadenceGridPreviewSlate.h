// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Widgets/SLeafWidget.h"

class CADENCEEDITOR_API SCadenceGridPreviewLine : public SLeafWidget
{
	SLATE_DECLARE_WIDGET(SCadenceGridPreviewLine, SLeafWidget)
public:	
	SLATE_BEGIN_ARGS( SCadenceGridPreviewLine )
		: _Points({FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f)})
		, _ColorAndOpacity( FLinearColor::White )
		, _Thickness(1.0f)
		{}
		
		SLATE_ATTRIBUTE(TArray<FVector2D>, Points)			
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
		SLATE_ATTRIBUTE(float, Thickness)
	SLATE_END_ARGS()

	SCadenceGridPreviewLine();
	
	void Construct( const FArguments& InArgs );
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
			FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
			bool bParentEnabled) const override;


protected:
	virtual FVector2D ComputeDesiredSize(float) const override;
	
private:
	TSlateAttribute<TArray<FVector2D>> PointsAttribute;
	TSlateAttribute<FSlateColor> ColorAndOpacityAttribute;
	TSlateAttribute<float> ThicknessAttribute;
};