// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CadenceAsset.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class UCadenceAsset;
class UCadenceGraph;
class UCadenceGraphEditor;

class FCadenceGraphApplication : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void InitEditor(const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost> InToolkitHost, UObject* InObject);

	UCadenceGraph* GetWorkingGraph() const { return WorkingAsset->GetGraph(); }
	UCadenceGraphEditor* GetWorkingGraphEditor() const { return WorkingGraphEditor; }
	TSharedPtr<SGraphEditor> GetSlateGraphEditor() const { return SlateGraphEditor.Pin(); }
	TSharedPtr<FUICommandList> GetCommandList();

	void SetSlateGraphEditor(const TSharedPtr<SGraphEditor>& InSlateGraphEditor) { SlateGraphEditor = InSlateGraphEditor; }

public: // FAssetEditorToolkit
	virtual FName GetToolkitFName() const override { return ToolkitFName; }
	virtual FText GetBaseToolkitName() const override { return BaseToolkitName; }
	virtual FString GetWorldCentricTabPrefix() const override { return WorldCentricTabPrefix; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return WorldCentricTabColorScale; }
	virtual FString GetDocumentationLink() const override { return DocumentationLink; }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;

private:
	void UpdateRuntimeGraph();
	void UpdateEditorGraph();
	
	// Begin UICommands
	void DeleteSelectedNodes() const;	
	bool CanDeleteSelectedNodes() const;

	void CutSelectedNodes();
	void CopySelectedNodes();
	void DuplicateSelectedNodes();
	bool HasValidSelection() const;
	
	void PasteClipboardNodes();
	bool HasValidNodesInClipboard() const;
	// End UICommands

public:
	static const FName ToolkitFName;
	static const FText BaseToolkitName;
	static const FString WorldCentricTabPrefix;
	static const FLinearColor WorldCentricTabColorScale;
	static const FString DocumentationLink;

private:
	/** Command list for the graph editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;
	
	UCadenceAsset* WorkingAsset = nullptr;
	UCadenceGraphEditor* WorkingGraphEditor = nullptr;
	TWeakPtr<SGraphEditor> SlateGraphEditor = nullptr;
};

class FCadenceGraphApplicationMode : public FApplicationMode
{
public:
	FCadenceGraphApplicationMode(TSharedPtr<FCadenceGraphApplication> InApplication);

	
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<FCadenceGraphApplication> Application;
	FWorkflowAllowedTabSet Tabs;

	
	/** Command list for the graph editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

public:
	static const FName ModeName;
	static const FName LayoutName;
};


class FCadenceGraphPrimaryTabFactory : public FWorkflowTabFactory
{
public:
	FCadenceGraphPrimaryTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FCadenceGraphApplication> Application;

public:
	static const FName Identifier;
};

class FCadenceGraphPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	FCadenceGraphPropertiesTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FCadenceGraphApplication> Application;

public:
	static const FName Identifier;
};