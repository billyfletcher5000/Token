// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceGraphSchemaActions.h"
#include "UObject/Object.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class UCadenceGraphEditor;
struct FGraphActionSort;
class UCadenceGraph;
struct FCadenceVariableAction;
struct FCadenceGraphAction;
class FCadenceGraphApplication;

class FCadenceGraphDetailsTabFactory : public FWorkflowTabFactory
{
public:
	FCadenceGraphDetailsTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FCadenceGraphApplication> Application;

public:
	static const FName Identifier;
};

class FCadenceGraphVariablesTabFactory : public FWorkflowTabFactory
{
public:
	FCadenceGraphVariablesTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FCadenceGraphApplication> Application;

public:
	static const FName Identifier;
};

class FCadenceGraphDetailsCommands : public TCommands<FCadenceGraphDetailsCommands>
{
public:
	/** Constructor */
	FCadenceGraphDetailsCommands() 
		: TCommands<FCadenceGraphDetailsCommands>(TEXT("FCadenceGraphDetails"), NSLOCTEXT("Contexts", "Cadence Graph Details", "Cadence Graph Details"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	// Basic operations
	TSharedPtr<FUICommandInfo> OpenExternalGraph;
	TSharedPtr<FUICommandInfo> FocusNode;
	TSharedPtr<FUICommandInfo> DeleteEntry;
	TSharedPtr<FUICommandInfo> PasteVariable;
	TSharedPtr<FUICommandInfo> GotoNativeVarDefinition;
	TSharedPtr<FUICommandInfo> AddNewVariable;
	
	// Add New Item
	/** Initialize commands */
	virtual void RegisterCommands() override;
};

class SCadenceGraphVariablesTabWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SCadenceGraphVariablesTabWidget ) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<FCadenceGraphApplication> InGraphApplication);
	~SCadenceGraphVariablesTabWidget();

	/* SWidget interface */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** Refreshes the graph action menu */
	void Refresh();
	
	/** Accessor for getting the current selection as a cadence graph */
	FCadenceGraphAction* SelectionAsGraph() const;

	/** Accessor for getting the current selection as a cadence variable */
	FCadenceVariableAction* SelectionAsVar() const;

	/** Accessor for determining if the current selection is a category*/
	bool SelectionIsCategory() const;
	
	UCadenceGraph* GetMainGraph() const { return CadenceGraph; }
	TWeakPtr<FCadenceGraphApplication> GetGraphApplication() { return ApplicationPtr; }

	/** Called to reset the search filter */
	void OnResetItemFilter();

	/** Selects an item by name in either the main graph action menu or the local one */
	void SelectItemByName(const FName& ItemName, ESelectInfo::Type SelectInfo = ESelectInfo::Direct, int32 SectionId = INDEX_NONE, bool bIsCategory = false);

	/** Clears the selection in the graph action menus */
	void ClearGraphActionMenuSelection();

	/** Initiates a rename on the selected action node, if possible */
	void OnRequestRenameOnActionNode();

	/** Expands any category with the associated name */
	void ExpandCategory(const FText& CategoryName);

	/** Move the category before the target category */
	bool MoveCategoryBeforeCategory( const FText& CategoryToMove, const FText& TargetCategory );
	
	/** Callbacks for Paste Commands */
	void OnPasteGeneric();
	bool CanPasteGeneric();
	
private:
	/** Creates widgets for the graph schema actions */
	TSharedRef<SWidget> OnCreateWidgetForAction(struct FCreateWidgetForActionData* const InCreateData);

	/** Callback used to populate all actions list in SGraphActionMenu */
	void CollectAllActions(FGraphActionListBuilderBase& OutAllActions);
	void CollectStaticSections(TArray<int32>& StaticSectionIDs);
	void GetChildGraphs(UEdGraph* EdGraph, int32 const SectionId, FGraphActionSort& SortList, const FText& ParentCategory = FText::GetEmpty()) const;
	
	/** Handles the visibility of the local action list */
	EVisibility GetLocalActionsListVisibility() const;

	/** Callbacks for the graph action menu */
	FReply OnActionDragged(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, const FPointerEvent& MouseEvent);
	FReply OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent);
	void OnActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions);
	void OnActionSelectedHelper(TSharedPtr<FEdGraphSchemaAction> InAction);
	void OnGlobalActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType);
	void OnActionDoubleClicked(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions);
	void ExecuteAction(TSharedPtr<FEdGraphSchemaAction> InAction);
	TSharedPtr<SWidget> OnContextMenuOpening();

	TSharedRef<SWidget> CreateAddNewMenuWidget();
	void BuildAddNewMenu(FMenuBuilder& MenuBuilder);
	TSharedRef<SWidget> CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag);

	void OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< struct FGraphActionNode > InAction );
	bool CanRequestRenameOnActionNode(TWeakPtr<struct FGraphActionNode> InSelectedNode) const;
	FText OnGetSectionTitle( int32 InSectionID );
	TSharedRef<SWidget> OnGetSectionWidget( TSharedRef<SWidget> RowWidget, int32 InSectionID );
	EVisibility OnGetSectionTextVisibility(TWeakPtr<SWidget> RowWidget, int32 InSectionID) const;
	FReply OnAddButtonClickedOnSection(int32 InSectionID);
	bool CanAddNewElementToSection(int32 InSectionID) const;

	bool HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const;
	
	/** Support functions for view options for Show Empty Sections */
	void OnToggleShowEmptySections();
	bool IsShowingEmptySections() const;	

	/** Helper function to open the selected graph */
	void OpenGraph(FDocumentTracker::EOpenDocumentCause InCause, bool bOpenExternalGraphInNewEditor = false);

	/** Callbacks for commands */
	void OnOpenExternalGraph();
	bool CanOpenExternalGraph() const;
	bool CanFocusOnNode() const;
	void OnFocusNode();
	void OnAddNewVariable();
	bool CanAddNewVariable() const;
	void OnDeleteEntry();
	bool CanDeleteEntry() const;
	bool CanRequestRenameOnActionNode() const;
	bool IsDuplicateActionVisible() const;
	bool CanDuplicateAction() const;
	void OnDuplicateAction();
	void OnCopy();
	bool CanCopy() const;
	void OnCut();
	bool CanCut() const;
	void OnPasteVariable();
	bool CanPasteVariable() const;

	/** Gets the currently selected Category or returns default category name */
	FText GetPasteCategory() const;

	/** Callback when the filter is changed, forces the action tree(s) to filter */
	void OnFilterTextChanged( const FText& InFilterText );

	/** Callback for the action trees to get the filter text */
	FText GetFilterText() const;

	/** Checks if the selected action has context menu */
	bool SelectionHasContextMenu() const;

	/** Returns the displayed category, if any, of a graph */
	FText GetGraphCategory(UEdGraph* InGraph) const;

	/** Helper function to delete a graph in the MyBlueprint window */
	void OnDeleteGraph(UEdGraph* InGraph, ECadenceGraphAction::Type);

	UEdGraph* GetFocusedGraph() const;

	/** Delegate to hook us into non-structural Blueprint object post-change events */
	void OnObjectPropertyChanged(UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent);

	/** Helper function indicating whehter we're in editing mode, and can modify the target blueprint */
	bool IsEditingMode() const;

private:
	/** List of UI Commands for this scope */
	TSharedPtr<FUICommandList> CommandList;

	/** Pointer back to the blueprint editor that owns us */
	TWeakPtr<FCadenceGraphApplication> ApplicationPtr;
	
	/** Graph Action Menu for displaying all our variables and functions */
	TSharedPtr<class SGraphActionMenu> GraphActionMenu;

	/** The filter box that handles filtering for both graph action menus. */
	TSharedPtr< SSearchBox > FilterBox;

	/** The blueprint being displayed: */
	UCadenceGraph* CadenceGraph = nullptr;

	/** The Ed Graph being displayed: */
	UCadenceGraphEditor* EdGraph = nullptr;


	/** The Kismet Inspector used to display properties: */
	//TWeakPtr<SKismetInspector> Inspector;

	/** Flag to indicate whether or not we need to refresh the panel */
	bool bNeedsRefresh = false;
	
	bool bIsShowingEmptySections = true;
};