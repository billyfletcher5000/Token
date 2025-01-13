// Fill out your copyright notice in the Description page of Project Settings.

#include "CadencePalette.h"

#include "CadenceGraphApplication.h"
#include "CadenceGraphSchema.h"
#include "CadenceGraphSchemaActions.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDocumentation.h"
#include "ISinglePropertyView.h"
#include "SPinTypeSelector.h"
#include "SPinTypeSelector.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Internationalization/Culture.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


class FAssetRegistryModule;
class FDragDropEvent;
struct FGeometry;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "BlueprintPalette"

/*******************************************************************************
* Static File Helpers
*******************************************************************************/

/** namespace'd to avoid collisions during unified builds */
namespace CadencePalette
{
	static FString const ConfigSection("CadenceEditor.Palette");
	static FString const FavoritesHeightConfigKey("FavoritesHeightRatio");
	static FString const LibraryHeightConfigKey("LibraryHeightRatio");
}

/**
 * A utility function intended to aid the construction of a specific cadence 
 * palette item. This looks at the item's associated action, and based off its  
 * type, retrieves an icon, color and tooltip for the slate widget.
 * 
 * @param  ActionIn		The action associated with the palette item you want an icon for.
 * @param  BrushOut		An output of the icon, best representing the specified action.
 * @param  ColorOut		An output color, further denoting the specified action.
 * @param  ToolTipOut	An output tooltip, best describing the specified action type.
 */
static void GetPaletteItemIcon(TSharedPtr<FEdGraphSchemaAction> ActionIn, FSlateBrush const*& BrushOut, FSlateColor& ColorOut, FText& ToolTipOut, FString& DocLinkOut, FString& DocExcerptOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut)
{
	// Default to tooltip based on action supplied
	ToolTipOut = ActionIn->GetTooltipDescription().IsEmpty() ? ActionIn->GetMenuDescription() : ActionIn->GetTooltipDescription();

	 if (ActionIn->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)ActionIn.Get();

		BrushOut = FCadenceGraphApplication::GetVarIconAndColorFromVariable(VarAction->GetVariable(), ColorOut, SecondaryBrushOut, SecondaryColorOut);
		ToolTipOut = FText::FromName(VarAction->GetVariable()->GetDisplayName());

	 	// TODO: Fix docs
		DocLinkOut = TEXT("Shared/Editor/Blueprint/VariableTypes");
		DocExcerptOut = VarAction->GetVariable()->GetDisplayName().ToString();
	}
	else
	{
		BrushOut = ActionIn->GetPaletteIcon();
		const FText ActionToolTip = ActionIn->GetPaletteToolTip();
		if(!ActionToolTip.IsEmpty())
		{
			ToolTipOut = ActionToolTip;
		}
	}
}

/**
 * Takes the existing tooltip and concats a path id (for the specified action) 
 * to the end.
 * 
 * @param  ActionIn		The action you want to show the path for.
 * @param  OldToolTip	The tooltip that you're replacing (we fold it into the new one)/
 * @return The newly created tooltip (now with the action's path tacked on the bottom).
 */
static TSharedRef<IToolTip> ConstructToolTipWithActionPath(TSharedPtr<FEdGraphSchemaAction> ActionIn, TSharedPtr<IToolTip> OldToolTip)
{
	TSharedRef<IToolTip> NewToolTip = OldToolTip.ToSharedRef();

	if (ActionIn->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)ActionIn.Get();
		
		static FTextBlockStyle PathStyle = FTextBlockStyle()
			.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.SetColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f));

		NewToolTip = SNew(SToolTip)

		// Emulate text-only tool-tip styling that SToolTip uses when no custom content is supplied.  We want node tool-tips to 
		// be styled just like text-only tool-tips
		.BorderImage( FCoreStyle::Get().GetBrush("ToolTip.BrightBackground") )
		.TextMargin(FMargin(11.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				OldToolTip->GetContentWidget()
			]

			+ SVerticalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			[
				SNew(STextBlock)
				.TextStyle( FAppStyle::Get(), "Documentation.SDocumentationTooltip")
				.Text(FText::FromString(VarAction->GetFriendlyVariableName()))
				//.TextStyle(&PathStyle)
			]
		];
	}

	return NewToolTip;
}


/*******************************************************************************
* SCadencePinTypeSelectorHelper
*******************************************************************************/
DECLARE_DELEGATE_OneParam(FOnPinTypeChanged, const FEdGraphPinType&)

void SCadencePinTypeSelectorHelper::Construct(const FArguments& InArgs, TWeakPtr<FCadenceVariableAction> InAction, const FChangeVariableTypeDelegate& InChangeTypeDelegate, bool InShowContainerTypeSelector)
{
	ActionPtr = InAction;
	ChangeTypeDelegate = InChangeTypeDelegate;
	bShowContainerTypeSelector = InShowContainerTypeSelector;
	
	Variable = nullptr;
	if (ActionPtr.IsValid())
	{
		Variable = ActionPtr.Pin()->GetVariable();
	}

	ConstructInternal(InArgs);
}

void SCadencePinTypeSelectorHelper::ConstructInternal(const FArguments& InArgs)
{
	OnTypeChanged = InArgs._OnTypeChanged;
	
	const UEdGraphSchema* Schema = GetDefault<UCadenceGraphSchema>();
	
	this->ChildSlot
	[
		SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(GetDefault<UCadenceGraphSchema>(), &UCadenceGraphSchema::GetVariableTypeTree))
		.Schema(Schema)
		.SchemaAction(ActionPtr)
		.TargetPinType(this, &SCadencePinTypeSelectorHelper::OnGetVarType)
		.OnPinTypeChanged(this, &SCadencePinTypeSelectorHelper::OnVarTypeChanged)
		.TypeTreeFilter(ETypeTreeFilter::None)
		.SelectorType(bShowContainerTypeSelector ? SPinTypeSelector::ESelectorType::Full : SPinTypeSelector::ESelectorType::Partial)
	];		
}

FEdGraphPinType SCadencePinTypeSelectorHelper::OnGetVarType() const
{
	if (Variable.IsValid())
	{
		const UCadenceGraphSchema* CadenceGraphSchema = GetDefault<UCadenceGraphSchema>();
		FEdGraphPinType Type;
		CadenceGraphSchema->ConvertVariableToPinType(Variable.Get(), Type);
		return Type;
	}
	else if(ActionPtr.IsValid())
	{
		return ActionPtr.Pin()->GetPinType();
	}
	return FEdGraphPinType();
}

void SCadencePinTypeSelectorHelper::OnVarTypeChanged(const FEdGraphPinType& InNewPinType)
{
	if (UCadenceVariable* Var = Variable.Get())
	{			
		ChangeTypeDelegate.ExecuteIfBound(Var, InNewPinType);
	}

	if (OnTypeChanged.IsBound())
	{
		OnTypeChanged.Execute(InNewPinType);
	}
}

/*******************************************************************************
* SPaletteItemVisibilityToggle
*******************************************************************************/

class SPaletteItemVisibilityToggle : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SPaletteItemVisibilityToggle ) {}
	SLATE_END_ARGS()

	/**
	 * Constructs a visibility-toggle widget (for variable actions only, so that 
	 * the user can modify the variable's "edit-on-instance" state).
	 * 
	 * @param  InArgs			A set of slate arguments, defined above.
	 * @param  ActionPtrIn		The FEdGraphSchemaAction that the parent item represents.
	 */
	void Construct(const FArguments& InArgs, TWeakPtr<FEdGraphSchemaAction> ActionPtrIn)
	{
		ActionPtr = ActionPtrIn;

		this->ChildSlot
		[
			SNew(SBorder)
			.Padding(0.0f)
			.BorderImage(FStyleDefaults::GetNoBrush())
			.Visibility(EVisibility::Visible)
			//.ForegroundColor(this, &SPaletteItemVisibilityToggle::GetVisibilityToggleColor)
			[
				SNew(SCheckBox)
				.ToolTipText(this, &SPaletteItemVisibilityToggle::GetVisibilityToggleToolTip)
				.OnCheckStateChanged(this, &SPaletteItemVisibilityToggle::OnVisibilityToggleFlipped)
				.IsChecked(this, &SPaletteItemVisibilityToggle::GetVisibilityToggleState)
				.Style(FAppStyle::Get(), "TransparentCheckBox")
				[
					SNew(SImage)
					.Image(this, &SPaletteItemVisibilityToggle::GetVisibilityIcon)
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			]
		];
	}

private:
	/**
	 * Used by this visibility-toggle widget to see if the property represented 
	 * by this item is visible outside of Kismet.
	 * 
	 * @return ECheckBoxState::Checked if the property is visible, false if not (or if the property wasn't found)
	 */
	ECheckBoxState GetVisibilityToggleState() const
	{
		TSharedPtr<FEdGraphSchemaAction> PaletteAction = ActionPtr.Pin();
		if ( PaletteAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId() )
		{
			TSharedPtr<FCadenceVariableAction> VarAction = StaticCastSharedPtr<FCadenceVariableAction>(PaletteAction);
			if (UCadenceVariable* VariableProperty = VarAction->GetVariable())
			{
				return VariableProperty->IsPublic() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			}
		}

		return ECheckBoxState::Unchecked;
	}

	/**
	 * Used by this visibility-toggle widget when the user makes a change to the
	 * checkbox (modifies the property represented by this item by flipping its
	 * edit-on-instance flag).
	 * 
	 * @param  InNewState	The new state that the user set the checkbox to.
	 */
	void OnVisibilityToggleFlipped(ECheckBoxState InNewState)
	{
		TSharedPtr<FEdGraphSchemaAction> PaletteAction = ActionPtr.Pin();
		if ( PaletteAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId() )
		{
			TSharedPtr<FCadenceVariableAction> VarAction = StaticCastSharedPtr<FCadenceVariableAction>(PaletteAction);
			if (UCadenceVariable* VariableProperty = VarAction->GetVariable())
			{
				VariableProperty->SetIsPublic(InNewState == ECheckBoxState::Checked);
			}
		}
	}

	/**
	 * Used by this visibility-toggle widget to convey the visibility of the 
	 * property represented by this item.
	 * 
	 * @return A image representing the variable's "edit-on-instance" state.
	 */
	const FSlateBrush* GetVisibilityIcon() const
	{
		return GetVisibilityToggleState() == ECheckBoxState::Checked ?
			FAppStyle::GetBrush( "Kismet.VariableList.ExposeForInstance" ) :
			FAppStyle::GetBrush( "Kismet.VariableList.HideForInstance" );
	}

	/**
	 * Used by this visibility-toggle widget to convey the visibility of the 
	 * property represented by this item (as well as the status of the 
	 * variable's tooltip).
	 * 
	 * @return A color denoting the item's visibility and tootip status.
	 */
	FSlateColor GetVisibilityToggleColor() const 
	{
		if ( GetVisibilityToggleState() != ECheckBoxState::Checked )
		{
			return FSlateColor::UseForeground();
		}
		else
		{
			static const FName TooltipExistsColor("Colors.AccentGreen");
			return FAppStyle::Get().GetSlateColor(TooltipExistsColor);
		}
	}

	/**
	 * Used by this visibility-toggle widget to supply the toggle with a tooltip
	 * representing the "edit-on-instance" state of the variable represented by 
	 * this item.
	 * 
	 * @return Tooltip text for this toggle.
	 */
	FText GetVisibilityToggleToolTip() const
	{
		FText ToolTipText = FText::GetEmpty();
		if ( GetVisibilityToggleState() != ECheckBoxState::Checked )
		{
			ToolTipText = LOCTEXT("VariablePrivacy_not_public_Tooltip", "Variable is not public and will not be editable on an instance of this Blueprint.");
		}
		else
		{
			ToolTipText = LOCTEXT("VariablePrivacy_is_public_Tooltip", "Variable is public and is editable on each instance of this Blueprint.");
		}
		return ToolTipText;
	}

private:
	/** The action that the owning palette entry represents */
	TWeakPtr<FEdGraphSchemaAction> ActionPtr;
};

/*******************************************************************************
* SCadencePaletteItem Public Interface
*******************************************************************************/

//------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCadencePaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, TWeakPtr<FCadenceGraphApplication> InCadenceApplication)
{
	Construct(InArgs, InCreateData, InCadenceApplication.Pin()->GetWorkingAsset(), InCadenceApplication);
}

void SCadencePaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UCadenceAsset* InAsset)
{
	Construct( InArgs, InCreateData, InAsset, TWeakPtr<FCadenceGraphApplication>() );
}

void SCadencePaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UCadenceAsset* InAsset, TWeakPtr<FCadenceGraphApplication> InCadenceApplication)
{
	OnRefreshRequested = InArgs._OnRefreshRequested;
	
	check(InCreateData->Action.IsValid());
	check(InAsset);

	Asset = InAsset;

	bShowClassInTooltip = InArgs._ShowClassInTooltip;	

	TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
	ActionPtr = InCreateData->Action;
	ApplicationPtr = InCadenceApplication;	
	
	// We differentiate enabled/read-only state here to not dim icons out unnecessarily, which in some situations
	// (like the right-click palette menu) is confusing to users.
	auto IsEditingEnabledLambda = [InCadenceApplication]()
	{ 
		if(InCadenceApplication.IsValid())
		{
			return InCadenceApplication.Pin()->InEditingMode();
		}

		return true;
	};

	TAttribute<bool> bIsEditingEnabled = TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateLambda(IsEditingEnabledLambda));

	// construct the icon widget
	FSlateBrush const* IconBrush   = FAppStyle::GetBrush(TEXT("NoBrush"));
	FSlateBrush const* SecondaryBrush = FAppStyle::GetBrush(TEXT("NoBrush"));
	FSlateColor        IconColor   = FSlateColor::UseForeground();
	FSlateColor        SecondaryIconColor   = FSlateColor::UseForeground();
	FText			   IconToolTip = GraphAction->GetTooltipDescription();
	FString			   IconDocLink, IconDocExcerpt;
	GetPaletteItemIcon(GraphAction, IconBrush, IconColor, IconToolTip, IconDocLink, IconDocExcerpt, SecondaryBrush, SecondaryIconColor);
	TSharedRef<SWidget> IconWidget = CreateIconWidget(IconToolTip, IconBrush, IconColor, IconDocLink, IconDocExcerpt, SecondaryBrush, SecondaryIconColor);
	IconWidget->SetEnabled(bIsEditingEnabled);

	// Enum representing the access specifier of this function or variable
	enum class EAccessSpecifier : uint8
	{
		None		= 0,
		Private		= 1,
		Protected	= 2,
		Public		= 3
	};

	EAccessSpecifier ActionAccessSpecifier = EAccessSpecifier::None;	

	// construct the text widget
	TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(InCreateData, false);

	// For Variables and Local Variables, we will convert the icon widget into a pin type selector.
	if (GraphAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{	
		TSharedPtr<FCadenceVariableAction> Action =StaticCastSharedPtr<FCadenceVariableAction>(GraphAction);
		
		UCadenceVariable* Variable = nullptr;
		if (Action.IsValid())
		{
			Variable = Action->GetVariable();
		}
		if (Variable)
		{
			IconWidget = SNew(SCadencePinTypeSelectorHelper, Action, FChangeVariableTypeDelegate::CreateSP(ApplicationPtr.Pin().ToSharedRef(), &FCadenceGraphApplication::ChangeVariableType), true)
				.IsEnabled(bIsEditingEnabled)
				.OnTypeChanged(this, &SCadencePaletteItem::OnPinTypeChanged);

			ActionAccessSpecifier = Variable->IsPublic() ? EAccessSpecifier::Public : EAccessSpecifier::Private;
		}
	}	

	FText AccessModifierText = FText::GetEmpty();

	switch (ActionAccessSpecifier)
	{
		case EAccessSpecifier::Public:
		{
			AccessModifierText = LOCTEXT("AccessModifierPublic", "public");
		}
		break;
		case EAccessSpecifier::Protected:
		{
			AccessModifierText = LOCTEXT("AccessModifierProtected", "protected");
		}
		break;
		case EAccessSpecifier::Private:
		{
			AccessModifierText = LOCTEXT("AccessModifierPrivate", "private");
		}
		break;
	}

	// Calculate a color so that the text gets brighter the more accessible the action is
	const bool AccessSpecifierEnabled = false;//(ActionAccessSpecifier != EAccessSpecifier::None);

	// Create the widget with an icon
	TSharedRef<SHorizontalBox> ActionBox = SNew(SHorizontalBox);


	auto CreateAccessSpecifierLambda = [&ActionBox, &AccessSpecifierEnabled, &AccessModifierText, &ActionAccessSpecifier]() {

		ActionBox.Get().AddSlot()
			.MaxWidth(50.f)
			.FillWidth(AccessSpecifierEnabled ? 0.4f : 0.0f)
			.Padding(FMargin(/* horizontal */ AccessSpecifierEnabled ? 6.0f : 0.0f, /* vertical */ 0.0f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				// Will only display text if we have a modifier level
				.IsEnabled(AccessSpecifierEnabled)
				.Text(AccessModifierText)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				// Bold if public
				.TextStyle(FAppStyle::Get(), ActionAccessSpecifier == EAccessSpecifier::Public ? "BlueprintEditor.AccessModifier.Public" : "BlueprintEditor.AccessModifier.Default")
			];
	};

	
	if (GraphAction->IsA(FCadenceVariableAction::StaticGetTypeId()))
	{
		if (ActionAccessSpecifier != EAccessSpecifier::None)
		{
			CreateAccessSpecifierLambda();
		}

		ActionBox.Get().AddSlot()
			.FillWidth(0.6f)
			.VAlign(VAlign_Center)
			.Padding(3.0f, 0.0f)
			[
				NameSlotWidget
			];

		ActionBox.Get().AddSlot()
			.FillWidth(0.4f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				IconWidget
			];

		ActionBox.Get().AddSlot()
			.AutoWidth()
			.Padding(FMargin(6.0f, 0.0f, 3.0f, 0.0f))
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SPaletteItemVisibilityToggle, ActionPtr)
				.IsEnabled(bIsEditingEnabled)
			];
	}
	else
	{
		ActionBox.Get().AddSlot()
			.AutoWidth()
			.Padding(FMargin(0.0f, 0.0f, 3.0f, 0.0f))
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SPaletteItemVisibilityToggle, ActionPtr)
				.IsEnabled(bIsEditingEnabled)
			];


		ActionBox.Get().AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				IconWidget
			];

		// Only add an access specifier if we have one
		if (ActionAccessSpecifier != EAccessSpecifier::None)
		{
			CreateAccessSpecifierLambda();
		}

		ActionBox.Get().AddSlot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			.Padding(/* horizontal */ 3.0f, /* vertical */ 3.0f)
			[
				NameSlotWidget
			];		
	}

	// Now, create the actual widget
	ChildSlot
	[
		ActionBox
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCadencePaletteItem::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (ApplicationPtr.IsValid())
	{
		SGraphPaletteItem::OnDragEnter(MyGeometry, DragDropEvent);
	}
}

/*******************************************************************************
* SCadencePaletteItem Private Methods
*******************************************************************************/

//------------------------------------------------------------------------------
TSharedRef<SWidget> SCadencePaletteItem::CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnlyIn)
{
	FName const ActionTypeId = InCreateData->Action->GetTypeId();

	FOnVerifyTextChanged OnVerifyTextChanged;
	FOnTextCommitted     OnTextCommitted;
		
	// default to our own rename methods
	OnVerifyTextChanged.BindSP(this, &SCadencePaletteItem::OnNameTextVerifyChanged);
	OnTextCommitted.BindSP(this, &SCadencePaletteItem::OnNameTextCommitted);


	// Copy the mouse delegate binding if we want it
	if( InCreateData->bHandleMouseButtonDown )
	{
		MouseButtonDownDelegate = InCreateData->MouseButtonDownDelegate;
	}

	TSharedPtr<SToolTip> ToolTipWidget = ConstructToolTipWidget();

	TSharedPtr<SOverlay> DisplayWidget;
	TSharedPtr<SInlineEditableTextBlock> EditableTextElement;
	SAssignNew(DisplayWidget, SOverlay)
		+SOverlay::Slot()
		[
			SAssignNew(EditableTextElement, SInlineEditableTextBlock)
				.Text(this, &SCadencePaletteItem::GetDisplayText)
				.HighlightText(InCreateData->HighlightText)
				.ToolTip(ToolTipWidget)
				.OnVerifyTextChanged(OnVerifyTextChanged)
				.OnTextCommitted(OnTextCommitted)
				.IsSelected(InCreateData->IsRowSelectedDelegate)
				.IsReadOnly(bIsReadOnlyIn)
		];
	InlineRenameWidget = EditableTextElement.ToSharedRef();

	InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

	if (false && ActionPtr.IsValid())
	{
		check(InlineRenameWidget.IsValid());
		TSharedPtr<IToolTip> ExistingToolTip = InlineRenameWidget->GetToolTip();

		DisplayWidget->AddSlot(0)
			[
				SNew(SHorizontalBox)
				.Visibility(EVisibility::Visible)
				.ToolTip(ConstructToolTipWithActionPath(ActionPtr.Pin(), ExistingToolTip))
			];
	}

	return DisplayWidget.ToSharedRef();
}

//------------------------------------------------------------------------------
FText SCadencePaletteItem::GetDisplayText() const
{
	if(ActionPtr.Pin()->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{		
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)ActionPtr.Pin().Get();
		return FText::FromName(VarAction->GetVariableName());
	}
	
	const UCadenceGraphSchema* CadenceGraphSchema = GetDefault<UCadenceGraphSchema>();
	if (MenuDescriptionCache.IsOutOfDate(CadenceGraphSchema))
	{
		MenuDescriptionCache.SetCachedText(ActionPtr.Pin()->GetMenuDescription(), CadenceGraphSchema);		
	}

	return MenuDescriptionCache;
}

//------------------------------------------------------------------------------
bool SCadencePaletteItem::OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage)
{
	FString TextAsString = InNewText.ToString();

	if(ActionPtr.Pin()->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{		
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)ActionPtr.Pin().Get();
		return VarAction->GetVariableName().ToString() != TextAsString;
	}
	
	return false;
}

//------------------------------------------------------------------------------
void SCadencePaletteItem::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	const FString NewNameString = NewText.ToString();
	const FName NewName = *NewNameString;
	
	if(ActionPtr.Pin()->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
	{
		FCadenceVariableAction* VarAction = (FCadenceVariableAction*)ActionPtr.Pin().Get();
		VarAction->GetVariable()->SetUserVariableName(NewName);

		ApplicationPtr.Pin()->Refresh();
	}
}

//------------------------------------------------------------------------------
FText SCadencePaletteItem::GetToolTipText() const
{
	TSharedPtr<FEdGraphSchemaAction> PaletteAction = ActionPtr.Pin();

	FText ToolTipText;
	FText ClassDisplayName;

	if (PaletteAction.IsValid())
	{
		// Default tooltip is taken from the action
		ToolTipText = PaletteAction->GetTooltipDescription().IsEmpty() ? PaletteAction->GetMenuDescription() : PaletteAction->GetTooltipDescription();

		if(PaletteAction->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
		{
			FCadenceVariableAction* VarAction = (FCadenceVariableAction*)PaletteAction.Get();
			if(UCadenceVariable* Variable = VarAction->GetVariable())
			ToolTipText = FText::FromName(Variable->GetUserVariableName());
		}
	}

	if (bShowClassInTooltip && !ClassDisplayName.IsEmpty())
	{
		ToolTipText = FText::Format(LOCTEXT("BlueprintItemClassTooltip", "{0}\nClass: {1}"), ToolTipText, ClassDisplayName);
	}

	return ToolTipText;
}

bool SCadencePaletteItem::IsSelected()
{
	TSharedPtr<FCadenceGraphApplication> GraphApplication = ApplicationPtr.Pin();
	TSharedPtr<FEdGraphSchemaAction> Action = ActionPtr.Pin();
	
	if (GraphApplication.IsValid() && Action.IsValid())
	{
		if (Action->GetTypeId() == FCadenceVariableAction::StaticGetTypeId())
		{
			FCadenceVariableAction* VarAction = (FCadenceVariableAction*)Action.Get();
			UCadenceVariable* Var = VarAction->GetVariable();
			
			return GraphApplication->GetSelectedDetailsView()->GetSelectedObjects().Contains(Var);
		}
		else if (Action->GetTypeId() == FCadenceGraphAction::StaticGetTypeId())
		{
			FCadenceGraphAction* GraphAction = (FCadenceGraphAction*)Action.Get();
		}
	}

	return false;
}

void SCadencePaletteItem::OnPinTypeChanged(const FEdGraphPinType&)
{
	OnRefreshRequested.ExecuteIfBound();
}

TSharedPtr<SToolTip> SCadencePaletteItem::ConstructToolTipWidget() const
{
	TSharedPtr<FEdGraphSchemaAction> PaletteAction = ActionPtr.Pin();

	// Setup the attribute for dynamically pulling the tooltip
	TAttribute<FText> TextAttribute;
	TextAttribute.Bind(this, &SCadencePaletteItem::GetToolTipText);

	TSharedRef< SToolTip > TooltipWidget = IDocumentation::Get()->CreateToolTip(TextAttribute, nullptr, FString(), FString());

	return TooltipWidget;
}

/*******************************************************************************
* SCadencePalette
*******************************************************************************/

//------------------------------------------------------------------------------
void SCadencePalette::Construct(const FArguments& InArgs, TWeakPtr<FCadenceGraphApplication> InCadenceApplication)
{
	const float NumProgressFrames = 2.0f;
	const float SecondsToWaitBeforeShowingProgressDialog = 0.25f;

	FScopedSlowTask SlowTask(NumProgressFrames, LOCTEXT("ConstructingPaletteTabContent", "Initializing Palette..."));
	SlowTask.MakeDialogDelayed(SecondsToWaitBeforeShowingProgressDialog);

	float FavoritesHeightRatio = 0.33f;
	GConfig->GetFloat(*CadencePalette::ConfigSection, *CadencePalette::FavoritesHeightConfigKey, FavoritesHeightRatio, GEditorPerProjectIni);
	float LibraryHeightRatio = 1.f - FavoritesHeightRatio;
	GConfig->GetFloat(*CadencePalette::ConfigSection, *CadencePalette::LibraryHeightConfigKey, LibraryHeightRatio, GEditorPerProjectIni);

	bool bUseLegacyLayout = false;
	GConfig->GetBool(*CadencePalette::ConfigSection, TEXT("bUseLegacyLayout"), bUseLegacyLayout, GEditorIni);

	/*
	SlowTask.EnterProgressFrame();
	TSharedRef<SWidget> FavoritesContent = SNew(SBlueprintFavoritesPalette, InCadenceApplication);

	SlowTask.EnterProgressFrame();
	TSharedRef<SWidget> LibraryContent = SNew(SBlueprintLibraryPalette, InCadenceApplication)
		.UseLegacyLayout(bUseLegacyLayout);

	if (bUseLegacyLayout)
	{
		LibraryWrapper = LibraryContent;

		this->ChildSlot
		[
			LibraryContent
		];
	}
	else 
	{
		LibraryContent->AddMetadata<FTagMetaData>(MakeShared<FTagMetaData>(TEXT("BlueprintPaletteLibrary")));
		FavoritesContent->AddMetadata<FTagMetaData>(MakeShared<FTagMetaData>(TEXT("BlueprintPaletteFavorites")));

		this->ChildSlot
		[
			SAssignNew(PaletteSplitter, SSplitter)
				.Orientation(Orient_Vertical)
				.OnSplitterFinishedResizing(this, &SCadencePalette::OnSplitterResized)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("FullBlueprintPalette")))

			+ SSplitter::Slot()
			.Value(FavoritesHeightRatio)
			[
				FavoritesContent
			]

			+ SSplitter::Slot()
			.Value(LibraryHeightRatio)
			[
				LibraryContent
			]
		];
	}	*/
}

//------------------------------------------------------------------------------
void SCadencePalette::OnSplitterResized() const
{
	FChildren const* const SplitterChildren = PaletteSplitter->GetChildren();
	for (int32 SlotIndex = 0; SlotIndex < SplitterChildren->Num(); ++SlotIndex)
	{
		SSplitter::FSlot const& SplitterSlot = PaletteSplitter->SlotAt(SlotIndex);

		if (SplitterSlot.GetWidget() == FavoritesWrapper)
		{
			GConfig->SetFloat(*CadencePalette::ConfigSection, *CadencePalette::FavoritesHeightConfigKey, SplitterSlot.GetSizeValue(), GEditorPerProjectIni);
		}
		else if (SplitterSlot.GetWidget() == LibraryWrapper)
		{
			GConfig->SetFloat(*CadencePalette::ConfigSection, *CadencePalette::LibraryHeightConfigKey, SplitterSlot.GetSizeValue(), GEditorPerProjectIni);
		}

	}
}


TSharedRef<IDetailCustomization> FCadenceVariableDetailCustomization::MakeInstance(FChangeVariableTypeDelegate InChangeTypeDelegate)
{
	return MakeShareable(new FCadenceVariableDetailCustomization(InChangeTypeDelegate));
}

void FCadenceVariableDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UCadenceVariable>> Variables = DetailBuilder.GetObjectsOfTypeBeingCustomized<UCadenceVariable>();
	if(Variables.Num() != 1)
		return;

	TSharedPtr<FCadenceVariableAction> Action = MakeShareable(new FCadenceVariableAction(Variables[0].Get()));
	
	IDetailCategoryBuilder& TypeCategory = DetailBuilder.EditCategoryAllowNone(TEXT("Variable Type"));

	TypeCategory.AddCustomRow(FText::FromString(TEXT("VariableType")))
		.NameContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Variable Type")))
			]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCadencePinTypeSelectorHelper, Action.ToWeakPtr(), ChangeTypeDelegate, true)
			]
		];
}

TSharedRef<IDetailCustomization> FCadenceVariableArrayDetailCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceVariableArrayDetailCustomization);
}

void FCadenceVariableArrayDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout(UCadenceVariable::StaticClass()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCadenceVariableArrayPropertyCustomization::MakeInstance));

	auto ValueProperty = DetailBuilder.GetProperty(TEXT("Value"));

	auto ArraysBeingCustomised = DetailBuilder.GetObjectsOfTypeBeingCustomized<UCadenceVariableArray>();
	TWeakObjectPtr<UCadenceVariableArray> Array = ArraysBeingCustomised[0];

	ArrayHandle = ValueProperty->AsArray();
	ArrayHandle->SetOnNumElementsChanged(FSimpleDelegate::CreateSP(this, &FCadenceVariableArrayDetailCustomization::OnArrayNumElementsChanged, Array, &DetailBuilder));	
}

void FCadenceVariableArrayDetailCustomization::OnArrayNumElementsChanged(TWeakObjectPtr<UCadenceVariableArray> InArray, IDetailLayoutBuilder* InDetailBuilder)
{
	if(!ArrayHandle.IsValid() || !InArray.IsValid())
		return;
	
	UClass* ArrayVariableClass = InArray->GetVariableClass();
	ensure(IsValid(ArrayVariableClass));
	
	TArray<TWeakPtr<UCadenceVariableArray>> ArrayObjects;

	uint32 NumElements = 0;
	ArrayHandle->GetNumElements(NumElements);

	bool AnyReplaced = false;
	for(uint32 Index = 0; Index < NumElements; ++Index)
	{
		auto ElementHandle = ArrayHandle->GetElement(Index);
		UObject* ElementUObject = nullptr;
		ElementHandle->GetValue(ElementUObject);
		
		if(!IsValid(ElementUObject) || ElementUObject->GetClass() != ArrayVariableClass)
		{
			UCadenceVariable* ReplacementVariable = NewObject<UCadenceVariable>(InArray.Get(), ArrayVariableClass);
			ElementHandle->SetValue(ReplacementVariable);
			AnyReplaced = true;
		}
	}

	InDetailBuilder->ForceRefreshDetails();
}

TSharedRef<IPropertyTypeCustomization> FCadenceVariablePropertyCustomization::MakeInstance(FChangeVariableTypeDelegate InChangeTypeDelegate, bool InShowContainerType)
{
	return MakeShareable(new FCadenceVariablePropertyCustomization(InChangeTypeDelegate, InShowContainerType));
}

void FCadenceVariablePropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

FDetailWidgetRow& FCadenceVariablePropertyCustomization::CreateVariableDisplay(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, UCadenceVariable* Variable, FChangeVariableTypeDelegate InChangeTypeDelegate, bool InShowContainerType)
{
	TSharedPtr<FCadenceVariableAction> Action = MakeShareable(new FCadenceVariableAction(Variable));

	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams Params;
	Params.NamePlacement = EPropertyNamePlacement::Hidden;
	auto NameElement = EditModule.CreateSingleProperty(Variable, FName(TEXT("UserVariableName")), Params);
	auto ValueElement = EditModule.CreateSingleProperty(Variable, FName(TEXT("Value")), Params);
	
	return ChildBuilder.AddCustomRow(FText::FromString(TEXT("Variable")))
			            .NameContent()
						[			
							PropertyHandle->CreatePropertyNameWidget()
						]
						.ValueContent()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.MinWidth(200)
							.Padding(0, 0, 10, 0)
							[
								NameElement.ToSharedRef()
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SCadencePinTypeSelectorHelper, Action.ToWeakPtr(), InChangeTypeDelegate, InShowContainerType)
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[				
								ValueElement.ToSharedRef()
							]
						];
}

void FCadenceVariablePropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
                                                              IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UObject* VariableObj = nullptr;
	PropertyHandle->GetValue(VariableObj);
	UCadenceVariable* Variable = Cast<UCadenceVariable>(VariableObj);
	check(Variable);
	
	CreateVariableDisplay(PropertyHandle, ChildBuilder, Variable, ChangeTypeDelegate, bShowContainerType);
}

TSharedRef<IPropertyTypeCustomization> FCadenceVariableArrayPropertyCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceVariableArrayPropertyCustomization);
}

void FCadenceVariableArrayPropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceVariableArrayPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// We have to retrieve the UObject of the prop as the properties generated are based off its base class UCadenceVariable and thus won't have the Value variable available
	UObject* PropValueUObject;
	PropertyHandle->GetValue(PropValueUObject);

	if(!IsValid(PropValueUObject))
		return;
	
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams Params;
	Params.NamePlacement = EPropertyNamePlacement::Hidden;
	auto Element = EditModule.CreateSingleProperty(PropValueUObject, FName(TEXT("Value")), Params);
	
	ChildBuilder.AddCustomRow(FText::FromString(TEXT("Value")))
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		Element.ToSharedRef()
	];
}

#undef LOCTEXT_NAMESPACE
