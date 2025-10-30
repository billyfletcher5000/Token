// Copyright (C) Billy Fletcher 2025


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

SLATE_IMPLEMENT_WIDGET(SCadenceGridPreviewPoint)
void SCadenceGridPreviewPoint::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Position", PositionAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Image", ImageAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ColorAndOpacity", ColorAndOpacityAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Radius", RadiusAttribute, EInvalidateWidgetReason::Paint);
}

SCadenceGridPreviewPoint::SCadenceGridPreviewPoint()  
	: PositionAttribute(*this, FVector2f(0.5f, 0.5f))
	, ImageAttribute(*this)
	, ColorAndOpacityAttribute(*this)
	, RadiusAttribute(*this)
{
}

void SCadenceGridPreviewPoint::Construct(const FArguments& InArgs)
{
	PositionAttribute.Assign(*this, InArgs._Position);
	ImageAttribute.Assign(*this, InArgs._Image);
	ColorAndOpacityAttribute.Assign(*this, InArgs._ColorAndOpacity);
	RadiusAttribute.Assign(*this, InArgs._Radius);
}

int32 SCadenceGridPreviewPoint::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* ImageBrush = ImageAttribute.Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const FLinearColor FinalColorAndOpacity( InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacityAttribute.Get().GetColor(InWidgetStyle) * ImageBrush->GetTint( InWidgetStyle ) );

		float Radius = RadiusAttribute.Get();
		FVector2f Position = PositionAttribute.Get();
		Position.Y = 1.0f - Position.Y;
		Position *= AllottedGeometry.GetLocalSize();
		
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(FVector2D(2.0f * Radius), FSlateLayoutTransform(Position - Radius)),
			ImageBrush,
			DrawEffects,
			FinalColorAndOpacity);		
	}

	return LayerId;
}

FVector2D SCadenceGridPreviewPoint::ComputeDesiredSize(float) const
{
	float Radius = RadiusAttribute.Get();
	return FVector2D(Radius * 2.0f);
}
