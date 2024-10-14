// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPreview/CadenceGridPreviewSlate.h"


SLATE_IMPLEMENT_WIDGET(SCadenceGridPreviewLine)
void SCadenceGridPreviewLine::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Points", PointsAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ColorAndOpacity", ColorAndOpacityAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Thickness", ThicknessAttribute, EInvalidateWidgetReason::Paint);
}

SCadenceGridPreviewLine::SCadenceGridPreviewLine()
	: PointsAttribute(*this)
	, ColorAndOpacityAttribute(*this)
	, ThicknessAttribute(*this)
{
}

void SCadenceGridPreviewLine::Construct(const FArguments& InArgs)
{
	PointsAttribute.Assign(*this, InArgs._Points);
	ColorAndOpacityAttribute.Assign(*this, InArgs._ColorAndOpacity);
	ThicknessAttribute.Assign(*this, InArgs._Thickness);
}

int32 SCadenceGridPreviewLine::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{	
	const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacityAttribute.Get().GetColor(InWidgetStyle));
	TArray<FVector2D> Points = PointsAttribute.Get();
	for(auto& Point : Points)
	{
		Point.Y = 1.0 - Point.Y; // Y axis is opposite direction in grid
		Point *= AllottedGeometry.GetLocalSize();
	}
	
	FSlateDrawElement::MakeLines(OutDrawElements,
								 LayerId,
								 AllottedGeometry.ToPaintGeometry(),
								 Points,
								 ESlateDrawEffect::None,
								 FinalColorAndOpacity,
								 true,
								 ThicknessAttribute.Get());

	return LayerId;
	//return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FVector2D SCadenceGridPreviewLine::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}
