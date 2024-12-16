// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSubPalette.h"

#include "CadenceGraphApplication.h"
#include "CadenceGraphSchemaActions.h"
#include "CadenceItemDragDropAction.h"
#include "CadencePalette.h"
#include "CoreGlobals.h"
#include "Delegates/Delegate.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "Layout/Children.h"
#include "Layout/Margin.h"
#include "Layout/Visibility.h"
#include "Math/Color.h"
#include "Misc/AssertionMacros.h"
#include "SGraphActionMenu.h"
#include "SlotBase.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "Templates/Casts.h"
#include "Templates/SubclassOf.h"
#include "Types/WidgetActiveTimerDelegate.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/UnrealNames.h"
#include "UObject/UnrealType.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SExpanderArrow.h"

class FDragDropOperation;
class SWidget;
struct FPointerEvent;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "CadenceSubPalette"

/*******************************************************************************
* Static File Helpers
*******************************************************************************/

/**
 * Checks to see if the user can drop the currently dragged action to place its
 * associated node in the graph.
 * 
 * @param  DropActionIn			The action that will be executed when the user drops the dragged item.
 * @param  HoveredGraphIn		A pointer to the graph that the user currently has the item dragged over.
 * @param  ImpededReasonOut		If this returns false, this will be filled out with a reason to present the user with.
 * @return True is the dragged palette item can be dropped where it currently is, false if not.
 */
static bool CanPaletteItemBePlaced(TSharedPtr<FEdGraphSchemaAction> DropActionIn, UEdGraph* HoveredGraphIn, FText& ImpededReasonOut)
{
	bool bCanBePlaced = true;
	if (!DropActionIn.IsValid())
	{
		bCanBePlaced = false;
		ImpededReasonOut = LOCTEXT("InvalidDropAction", "Invalid action for placement");
	}
	else if (HoveredGraphIn == NULL)
	{
		bCanBePlaced = false;
		ImpededReasonOut = LOCTEXT("DropOnlyInGraph", "Nodes can only be placed inside the blueprint graph");
	}
	else if (DropActionIn->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)DropActionIn.Get();
		return IsValid(VarAction->GetVariable());
	}

	return bCanBePlaced;
}

/*******************************************************************************
* FBlueprintPaletteCommands
*******************************************************************************/

class FCadencePaletteCommands : public TCommands<FCadencePaletteCommands>
{
public:
	FCadencePaletteCommands() : TCommands<FCadencePaletteCommands>
		( "CadencePalette"
		, LOCTEXT("PaletteContext", "Palette")
		, NAME_None
		, FAppStyle::GetAppStyleSetName() )
	{
	}

	TSharedPtr<FUICommandInfo> RefreshPalette;

	/**
	 * Registers context menu commands for the blueprint palette.
	 */
	virtual void RegisterCommands() override
	{
		UI_COMMAND(RefreshPalette, "Refresh List", "Refreshes the list of nodes.", EUserInterfaceActionType::Button, FInputChord());
	}
};

/*******************************************************************************
* SCadenceSubPalette Public Interface
*******************************************************************************/

//------------------------------------------------------------------------------
SCadenceSubPalette::~SCadenceSubPalette()
{
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::Construct(FArguments const& InArgs, TWeakPtr<FCadenceGraphApplication> InApplicationPtr)
{
	bIsActiveTimerRegistered = false;
	ApplicationPtr = InApplicationPtr;

	struct LocalUtils
	{
		static TSharedRef<SExpanderArrow> CreateCustomExpander(const FCustomExpanderData& ActionMenuData, bool bShowFavoriteToggle)
		{
			TSharedPtr<SExpanderArrow> CustomExpander;
			SAssignNew(CustomExpander, SExpanderArrow, ActionMenuData.TableRow);
			
			return CustomExpander.ToSharedRef();
		}
	};

	ChildSlot
 	[
		SNew(SBorder)
		.Padding(2.f)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew( SVerticalBox )
			+SVerticalBox::Slot()
				.AutoHeight()
				.Padding( 0.f, 2.f, 0.f, 0.f )
			[
				ConstructHeadingWidget(InArgs._Icon.Get(), InArgs._Title.Get(), InArgs._ToolTipText.Get())
			]

			+SVerticalBox::Slot()
			[
				SNew(SOverlay)
				+SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
				[
					SAssignNew(GraphActionMenu, SGraphActionMenu)
						.OnCreateWidgetForAction(this, &SCadenceSubPalette::OnCreateWidgetForAction)
						.OnActionDragged(this, &SCadenceSubPalette::OnActionDragged)
						.OnCollectAllActions(this, &SCadenceSubPalette::CollectAllActions)
						.OnContextMenuOpening(this, &SCadenceSubPalette::ConstructContextMenuWidget)
						.OnCreateCustomRowExpander_Static(&LocalUtils::CreateCustomExpander, InArgs._ShowFavoriteToggles.Get())
				]
			]
		]
	];

	CommandList = MakeShareable(new FUICommandList);
	// has to come after GraphActionMenu has been set
	BindCommands(CommandList);
}

//------------------------------------------------------------------------------
EActiveTimerReturnType SCadenceSubPalette::TriggerRefreshActionsList(double InCurrentTime, float InDeltaTime)
{
	RefreshActionsList(true);
	bIsActiveTimerRegistered = false;
	return EActiveTimerReturnType::Stop;
}

//------------------------------------------------------------------------------
UCadenceAsset* SCadenceSubPalette::GetCadenceAsset() const
{
	UCadenceAsset* AssetBeingEdited = NULL;
	if (ApplicationPtr.IsValid())
	{
		AssetBeingEdited = ApplicationPtr.Pin()->GetWorkingAsset();
	}
	return AssetBeingEdited;
}

//------------------------------------------------------------------------------
TSharedPtr<FEdGraphSchemaAction> SCadenceSubPalette::GetSelectedAction() const
{
	TArray< TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	return TSharedPtr<FEdGraphSchemaAction>( (SelectedActions.Num() > 0) ? SelectedActions[0] : NULL );
}

/*******************************************************************************
* Protected SCadenceSubPalette Methods
*******************************************************************************/

//------------------------------------------------------------------------------
void SCadenceSubPalette::RefreshActionsList(bool bPreserveExpansion)
{
	// Prevent refreshing the palette if we're in PIE
	if( !GIsPlayInEditorWorld )
	{
		SGraphPalette::RefreshActionsList(bPreserveExpansion);
	}
}

//------------------------------------------------------------------------------
TSharedRef<SWidget> SCadenceSubPalette::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(SCadencePaletteItem, InCreateData, ApplicationPtr.Pin());
}

//------------------------------------------------------------------------------
FReply SCadenceSubPalette::OnActionDragged( const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent )
{
	if( InActions.Num() > 0 && InActions[0].IsValid() )
	{
		TSharedPtr<FEdGraphSchemaAction> InAction = InActions[0];

		auto CanNodeBePlacedDelegate = FCadenceVariableItemDragDropAction::FCanBeDroppedDelegate::CreateStatic(&CanPaletteItemBePlaced);

		if(InAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
		{
			FCadenceVariableAction* VarAction = (FCadenceVariableAction*)InAction.Get();
			if (UCadenceVariable* Var = VarAction->GetVariable())
			{
				FCadenceGraphApplication* App = ApplicationPtr.Pin().Get();
				return FReply::Handled().BeginDragDrop(FCadenceVariableItemDragDropAction::New(InAction, Var, App->GetWorkingAsset(), ApplicationPtr.Pin()));
			}
		}
		else
		{
			return FReply::Handled().BeginDragDrop(FCadenceItemDragDropAction::New(InAction, CanNodeBePlacedDelegate));
		}
	}

	return FReply::Unhandled();
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::BindCommands(TSharedPtr<FUICommandList> CommandListIn) const
{
	FCadencePaletteCommands::Register();
	FCadencePaletteCommands const& PaletteCommands = FCadencePaletteCommands::Get();

	CommandListIn->MapAction(
		PaletteCommands.RefreshPalette,
		FExecuteAction::CreateSP(const_cast<SCadenceSubPalette*>(this), &SCadenceSubPalette::RefreshActionsList, /*bPreserveExpansion =*/true)
	);
}

//------------------------------------------------------------------------------
TSharedPtr<SWidget> SCadenceSubPalette::ConstructContextMenuWidget() const
{
	FMenuBuilder MenuBuilder(/* bInShouldCloseWindowAfterMenuSelection =*/true, CommandList);
	GenerateContextMenuEntries(MenuBuilder);
	return MenuBuilder.MakeWidget();
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::GenerateContextMenuEntries(FMenuBuilder& MenuBuilder) const
{
	FCadencePaletteCommands const& PaletteCommands = FCadencePaletteCommands::Get();
	MenuBuilder.AddMenuEntry(PaletteCommands.RefreshPalette);
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::RequestRefreshActionsList()
{
	if (!bIsActiveTimerRegistered)
	{
		bIsActiveTimerRegistered = true;
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SCadenceSubPalette::TriggerRefreshActionsList));
	}
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::OnDatabaseActionsUpdated(UObject* /*ActionsKey*/)
{
	RequestRefreshActionsList();
}

//------------------------------------------------------------------------------
void SCadenceSubPalette::OnDatabaseActionsRemoved(UObject* ActionsKey)
{
	RequestRefreshActionsList();	
}

/*******************************************************************************
* Private SCadenceSubPalette Methods
*******************************************************************************/

//------------------------------------------------------------------------------
void SCadenceSubPalette::ForceRefreshActionList()
{
	RefreshActionsList(/*bPreserveExpansion =*/true);
}

//------------------------------------------------------------------------------
TSharedRef<SVerticalBox> SCadenceSubPalette::ConstructHeadingWidget(FSlateBrush const* const Icon, FText const& TitleText, FText const& ToolTipText)
{
	TSharedPtr<SToolTip> ToolTipWidget;
	SAssignNew(ToolTipWidget, SToolTip).Text(ToolTipText);

	static FTextBlockStyle TitleStyle = FTextBlockStyle()
		.SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 10))
		.SetColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f));

	return SNew(SVerticalBox)
		.ToolTip(ToolTipWidget)
		// so we still get tooltip text for an empty SHorizontalBox
		.Visibility(EVisibility::Visible) 
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(2.f, 2.f)
			[
				SNew(SImage).Image(Icon)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(2.f, 2.f)
			[
				SNew(STextBlock)
				.Text(TitleText)
				.TextStyle(&TitleStyle)
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 2.f, 0.f, 5.f)
		[
			SNew(SBorder)
			// use the border's padding to actually create the horizontal line
			.Padding(1.f)
			.BorderImage(FAppStyle::GetBrush(TEXT("Menu.Separator")))
		];	
}

#undef LOCTEXT_NAMESPACE
