// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class UCadenceGraph;

class FCadenceGraphApplication : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void InitEditor(const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost> InToolkitHost, UObject* InObject);

	UCadenceGraph* GetWorkingAsset() { return WorkingAsset; }

public: // FAssetEditorToolkit
	virtual FName GetToolkitFName() const override { return ToolkitFName; }
	virtual FText GetBaseToolkitName() const override { return BaseToolkitName; }
	virtual FString GetWorldCentricTabPrefix() const override { return WorldCentricTabPrefix; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return WorldCentricTabColorScale; }
	virtual FString GetDocumentationLink() const override { return DocumentationLink; }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;

public:
	static const FName ToolkitFName;
	static const FText BaseToolkitName;
	static const FString WorldCentricTabPrefix;
	static const FLinearColor WorldCentricTabColorScale;
	static const FString DocumentationLink;

private:
	UCadenceGraph* WorkingAsset = nullptr;
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