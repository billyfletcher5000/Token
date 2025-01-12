// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "SGraphPalette.h"
#include "UObject/Object.h"

struct FCadenceVariableAction;
class UCadenceVariable;
class UCadenceVariableArray;
class UCadenceAsset;
class FCadenceGraphApplication;

DECLARE_DELEGATE(FOnRefreshRequested)
DECLARE_DELEGATE_TwoParams(FChangeVariableTypeDelegate, UCadenceVariable* /*Variable*/, const FEdGraphPinType& /*NewPinType*/)

/** Widget for displaying a single item  */
class SCadencePaletteItem : public SGraphPaletteItem
{
public:
	SLATE_BEGIN_ARGS( SCadencePaletteItem )
			: _ShowClassInTooltip(false)
		{}

		SLATE_ARGUMENT(bool, ShowClassInTooltip)
		SLATE_EVENT(FOnRefreshRequested, OnRefreshRequested);
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
	
	bool IsSelected();

	void OnPinTypeChanged(const FEdGraphPinType&);
	
private:
	/** True if the class should be displayed in the tooltip */
	bool bShowClassInTooltip;

	UCadenceAsset* Asset;

	/** Pointer back to the blueprint editor that owns this */
	TWeakPtr<FCadenceGraphApplication> ApplicationPtr;

	/** Cache for the MenuDescription to be displayed for this item */
	FNodeTextCache MenuDescriptionCache;

	FOnRefreshRequested OnRefreshRequested;
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

/*******************************************************************************
* SCadencePinTypeSelectorHelper
*******************************************************************************/
DECLARE_DELEGATE_OneParam(FOnPinTypeChanged, const FEdGraphPinType&)

class SCadencePinTypeSelectorHelper : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SCadencePinTypeSelectorHelper ) {}
		SLATE_EVENT(FOnPinTypeChanged, OnTypeChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<FCadenceVariableAction> InAction, const FChangeVariableTypeDelegate& InChangeTypeDelegate, bool InShowContainerTypeSelector = false);

private:
	void ConstructInternal(const FArguments& InArgs);	
	FEdGraphPinType OnGetVarType() const;
	void OnVarTypeChanged(const FEdGraphPinType& InNewPinType);

private:
	/** The action that the owning palette entry represents */
	TWeakPtr<FCadenceVariableAction> ActionPtr;

	/** Variable Property to change the type of */
	TWeakObjectPtr<UCadenceVariable> Variable;
	
	FChangeVariableTypeDelegate ChangeTypeDelegate;

	bool bShowContainerTypeSelector = false;

	/** Event when type has changed */
	FOnPinTypeChanged OnTypeChanged;
};

class FCadenceVariableDetailCustomization : public IDetailCustomization
{
public:
	FCadenceVariableDetailCustomization(const FChangeVariableTypeDelegate& InChangeTypeDelegate) : ChangeTypeDelegate(InChangeTypeDelegate) {}
	static TSharedRef<IDetailCustomization> MakeInstance(FChangeVariableTypeDelegate InChangeTypeDelegate);
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	FChangeVariableTypeDelegate ChangeTypeDelegate;
};

class FCadenceVariableArrayDetailCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:	
	void OnArrayNumElementsChanged(TWeakObjectPtr<UCadenceVariableArray> InArray, IDetailLayoutBuilder* InDetailBuilder);

	TSharedPtr<IPropertyHandleArray> ArrayHandle;
};

class FCadenceVariablePropertyCustomization : public IPropertyTypeCustomization
{
public:
	FCadenceVariablePropertyCustomization(const FChangeVariableTypeDelegate& InChangeTypeDelegate, const bool& InShowContainerType) : ChangeTypeDelegate(InChangeTypeDelegate), bShowContainerType(InShowContainerType) {}
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(FChangeVariableTypeDelegate InChangeTypeDelegate, bool InShowContainerType);
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	static FDetailWidgetRow& CreateVariableDisplay(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, UCadenceVariable* Variable, FChangeVariableTypeDelegate InChangeTypeDelegate, bool InShowContainerType);
protected:
	FChangeVariableTypeDelegate ChangeTypeDelegate;
	bool bShowContainerType = true;
};

class FCadenceVariableArrayPropertyCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
};