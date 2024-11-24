// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPalette.h"
#include "UObject/Object.h"
#include "CadencePalette.generated.h"


class UCadenceAsset;
class FCadenceGraphApplication;

/** Widget for displaying a single item  */
class SCadencePaletteItem : public SGraphPaletteItem
{
public:
	SLATE_BEGIN_ARGS( SCadencePaletteItem )
			: _ShowClassInTooltip(false)
		{}

		SLATE_ARGUMENT(bool, ShowClassInTooltip)
	SLATE_END_ARGS()

	/**
	 * Creates the slate widget to be place in a palette.
	 * 
	 * @param  InArgs				A set of slate arguments, defined above.
	 * @param  InCreateData			A set of data associated with a FEdGraphSchemaAction that this item represents.
	 * @param  InCadenceApplication	A pointer to the cadence application editor that the palette belongs to.
	 */
	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, TWeakPtr<FCadenceGraphApplication> InCadenceApplication);
	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UCadenceAsset* InAsset);

private:
	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData, UCadenceAsset* InAsset, TWeakPtr<FCadenceGraphApplication> InCadenceApplication);

	// SWidget Interface
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	// End of SWidget Interface

	// SGraphPaletteItem Interface
	virtual TSharedRef<SWidget> CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnly ) override;
	virtual FText GetDisplayText() const override;
	virtual bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage) override;
	virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override;
	// End of SGraphPaletteItem Interface

	/**
	 * Creates a tooltip widget based off the specified action (attempts to 
	 * mirror the tool-tip that would be found on the node once it's placed).
	 * 
	 * @return A new slate widget to be used as the tool tip for this item's text element.
	 */
	TSharedPtr<SToolTip> ConstructToolTipWidget() const;

	/** Returns the up-to-date tooltip for the item */
	FText GetToolTipText() const;
private:
	/** True if the class should be displayed in the tooltip */
	bool bShowClassInTooltip;

	UCadenceAsset* Asset;

	/** Pointer back to the blueprint editor that owns this */
	TWeakPtr<FCadenceGraphApplication> ApplicationPtr;

	/** Cache for the MenuDescription to be displayed for this item */
	FNodeTextCache MenuDescriptionCache;
};


/*******************************************************************************
* SBlueprintPalette
*******************************************************************************/

class SCadencePalette : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SCadencePalette ) {};
	SLATE_END_ARGS()

	/**
	 * Creates the slate widget that represents a list of available actions for 
	 * the specified blueprint.
	 * 
	 * @param  InArgs				A set of slate arguments, defined above.
	 * @param  InCadenceApplication	A pointer to the blueprint editor that this palette belongs to.
	 */
	void Construct(const FArguments& InArgs, TWeakPtr<FCadenceGraphApplication> InCadenceApplication);

private:
	/**
	 * Saves off the user's new sub-palette configuration (so as to not annoy 
	 * them by reseting it every time they open the blueprint editor). 
	 */
	void OnSplitterResized() const;

	TSharedPtr<SWidget> FavoritesWrapper;
	TSharedPtr<SSplitter> PaletteSplitter;
	TSharedPtr<SWidget> LibraryWrapper;
};

