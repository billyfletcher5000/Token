// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphGridPreviewCommandDecorators.h"

#include "CadenceEditorModule.h"
#include "Graph/Nodes/GridPreview/CadenceGridPreviewDrawCommand.h"
#include "GridPreview/CadenceGridPreviewSlate.h"
#include "Styling/SlateStyleRegistry.h"


TSubclassOf<UCadenceGridPreviewDrawCommand> UCadenceGraphGridPreviewPointCommandDecorator::GetTargetCommandType() const
{
	return UCadenceGridPreviewDrawPointCommand::StaticClass();
}

TSharedRef<SWidget> UCadenceGraphGridPreviewPointCommandDecorator::CreateVisualDisplay(UCadenceGridPreviewDrawCommand* InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings)
{
	const UCadenceGridPreviewDrawPointCommand* DrawPointCommand = Cast<UCadenceGridPreviewDrawPointCommand>(InDrawCommand);
	TArray<FVector2D> Points = {DrawPointCommand->Position};
	
	return SNew(SCadenceGridPreviewLine)
			.Points(Points)
			.Thickness(DrawPointCommand->Size)
			.ColorAndOpacity(FLinearColor::Red);
}

TSubclassOf<UCadenceGridPreviewDrawCommand> UCadenceGraphGridPreviewLineCommandDecorator::GetTargetCommandType() const
{
	return UCadenceGridPreviewDrawLineCommand::StaticClass();
}

TSharedRef<SWidget> UCadenceGraphGridPreviewLineCommandDecorator::CreateVisualDisplay(
	UCadenceGridPreviewDrawCommand* InDrawCommand, const FCadenceGridPreviewDrawSettings& InSettings)
{
	const UCadenceGridPreviewDrawLineCommand* DrawLineCommand = Cast<UCadenceGridPreviewDrawLineCommand>(InDrawCommand);
	TArray<FVector2D> Points = {DrawLineCommand->PositionStart, DrawLineCommand->PositionEnd};
	
	return SNew(SCadenceGridPreviewLine)
			.Points(Points)
			.Thickness(DrawLineCommand->Thickness)
			.ColorAndOpacity(FLinearColor::Blue);
}
