// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphEditorGridNode.h"

#include "CadenceEditorModule.h"
#include "CadenceGraphGridPreviewCommandDecorators.h"
#include "CadenceGridNodePreviewSubsystem.h"
#include "GraphEditorSettings.h"
#include "SGraphPin.h"
#include "Graph/Nodes/CadenceGridNodes.h"
#include "Graph/Nodes/GridPreview/CadenceGridPreviewDrawCommand.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr<SGraphNode> UCadenceGraphEditorGridNode::CreateVisualWidget()
{
	return SNew(SCadenceGraphGridNode, this);
}

void UCadenceGraphEditorGridNode::GetPreviewDrawCommands(TArray<UCadenceGridPreviewDrawCommand*>& InDrawCommandList)
{
	ICadenceGraphGridCommandProvider* Provider = Cast<ICadenceGraphGridCommandProvider>(GetRuntimeGraphNode());
	if(ensure(Provider))
	{
		Provider->GetPreviewDrawCommands(InDrawCommandList);
	}	
}

void SCadenceGraphGridNode::Construct(const FArguments& InArgs, UCadenceGraphEditorGridNode* InNode)
{
	GraphNode = InNode;
	GridNode = InNode;

	SetCursor(EMouseCursor::Crosshairs);

	UpdateGraphNode();
}

void SCadenceGraphGridNode::CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox)
{
	if (GraphNode && MainBox.IsValid())
	{
		// Count the number of visible input pins on the left
		int32 LeftPinCount = 0;
		if (GraphNode->AdvancedPinDisplay == ENodeAdvancedPins::Hidden)
		{
			// Advanced view pins are hidden so exclude them from the pin count
			for (int32 i = 0; i < InputPins.Num(); ++i)
			{
				const UEdGraphPin* PinObj = InputPins[i]->GetPinObj();
				if (!PinObj->bAdvancedView)
				{
					LeftPinCount++;
				}
			}
		}
		else
		{
			LeftPinCount = InputPins.Num();
		}

		int32 RightPinCount = OutputPins.Num();

		const float NegativeHPad = FMath::Max<float>(-Settings->PaddingTowardsNodeEdge, 0.0f);
		const float ExtraPad = 0.0f;

		// Place preview widget based on where the least pins are
		if ((LeftPinCount < RightPinCount) || (RightPinCount == 0))
		{
			LeftNodeBox->AddSlot()
			.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				CreatePreviewWidget()
			];
		}
		else if (LeftPinCount > RightPinCount)
		{
			RightNodeBox->AddSlot()
			.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
			.AutoHeight()
			.HAlign(HAlign_Right)
			[
				CreatePreviewWidget()
			];
		}
		else
		{
			MainBox->AddSlot()
			.Padding(Settings->GetNonPinNodeBodyPadding())
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					CreatePreviewWidget()
				]
			];
		}
	}
}

void SCadenceGraphGridNode::SetDefaultTitleAreaWidget(TSharedRef<SOverlay> DefaultTitleAreaWidget)
{
	if (GridNode->CanDisplayGridPreview())
	{
		DefaultTitleAreaWidget->AddSlot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(FMargin(5))
		[
			SNew(SCheckBox)
			.OnCheckStateChanged( this, &SCadenceGraphGridNode::OnPreviewExpandedChanged )
			.IsChecked( GridNode->IsGridPreviewExpanded() )
			.Cursor(EMouseCursor::Default)
			.Style(FAppStyle::Get(), "Graph.Node.AdvancedView")
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SImage)
					.Image(GetGridPreviewArrow())
				]
			]
		];
	}
}

TSharedRef<SWidget> SCadenceGraphGridNode::CreatePreviewWidget()
{
	if (GridNode->CanDisplayGridPreview() && GridNode->IsGridPreviewExpanded())
	{
		const float ExpressionPreviewSize = 106.0f;
		const float CentralPadding = 5.0f;

		TSharedRef<SOverlay> Overlay = SNew(SOverlay)
										+ SOverlay::Slot()
										[
											CreateGridBaseWidget()
										];

		// TODO: This is passed by reference under the idea that nodes can rely on previous nodes to communicate movement etc in the future
		// TODO: but is pretty placeholder and probably not the best way to go about it.
		TArray<UCadenceGridPreviewDrawCommand*> DrawCommands; 
		GridNode->GetPreviewDrawCommands(DrawCommands);

		if(DrawCommands.Num() > 0)
		{
			FCadenceGridPreviewDrawSettings DrawSettings;
			constexpr float HorizontalPadding = 4.0f;
			constexpr float VerticalPadding = 4.0f;
			DrawSettings.Padding.Left = DrawSettings.Padding.Right = HorizontalPadding;
			DrawSettings.Padding.Top = DrawSettings.Padding.Bottom = VerticalPadding;
			
			UCadenceGridNodePreviewSubsystem* PreviewSubsystem = GEditor->GetEditorSubsystem<UCadenceGridNodePreviewSubsystem>();
			ensure(PreviewSubsystem);
			
			for (UCadenceGridPreviewDrawCommand* Command : DrawCommands)
			{
				UCadenceGraphGridPreviewCommandDecorator* Decorator = PreviewSubsystem->GetDecoratorForCommandType(Command->GetClass());
				ensure(Decorator);

				Overlay->AddSlot() [
					Decorator->CreateVisualDisplay(Command, DrawSettings)
				];
			}
		}

		Overlay->AddSlot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(0.0f,0.0f,8.0f,0.0f)
				[
					SNew(STextBlock)
					.ShadowColorAndOpacity(FLinearColor::Black)
					.ShadowOffset(FVector2D(1.0f, 1.0f))
					.Text(this, &SCadenceGraphGridNode::GetPreviewOverlayText)
				];		

		return SNew(SBox)
			.WidthOverride(ExpressionPreviewSize)
			.HeightOverride(ExpressionPreviewSize)
			.MaxAspectRatio(1.0f)
			.MaxDesiredHeight(ExpressionPreviewSize)
			.Visibility(GridNode->IsGridPreviewExpanded() ? EVisibility::Visible : EVisibility::Collapsed)
			[
				SNew(SBorder)
				.Padding(CentralPadding)
				.BorderImage( FAppStyle::GetBrush("NoBorder") )
				[
					Overlay					
				]
			];
	}

	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SCadenceGraphGridNode::CreateGridBaseWidget()
{
	const ISlateStyle* SlateStyle = FSlateStyleRegistry::FindSlateStyle(FCadenceEditorModule::StyleSetName);
	return SNew(SImage)
		.Image(SlateStyle->GetBrush("NodePreview.CadenceGrid"));	
}

void SCadenceGraphGridNode::OnPreviewExpandedChanged(ECheckBoxState InCheckBoxState)
{
	GridNode->SetGridPreviewExpanded(InCheckBoxState == ECheckBoxState::Checked);
	UpdateGraphNode();
}

const FSlateBrush* SCadenceGraphGridNode::GetGridPreviewArrow()
{	
	return FAppStyle::GetBrush(GridNode->IsGridPreviewExpanded() ? TEXT("Icons.ChevronDown") : TEXT("Icons.ChevronUp"));
}

FText SCadenceGraphGridNode::GetPreviewOverlayText() const
{
	return GridNode->GetPreviewOverlayText();
}
