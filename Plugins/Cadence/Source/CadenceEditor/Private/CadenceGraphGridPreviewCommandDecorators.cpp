// Copyright (C) Billy Fletcher 2025


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
	
	const ISlateStyle* SlateStyle = FSlateStyleRegistry::FindSlateStyle(FCadenceEditorModule::StyleSetName);
		
	return SNew(SCadenceGridPreviewPoint)
			.Position(DrawPointCommand->Position)
			.Image(SlateStyle->GetBrush("NodePreview.GridPoint"))
			.Radius(DrawPointCommand->Size)
			.ColorAndOpacity(DrawPointCommand->Color);
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
			.ColorAndOpacity(DrawLineCommand->Color);
}
