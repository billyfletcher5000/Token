// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceGraphApplication.h"

const FName FCadenceGraphApplication::ToolkitFName = FName(TEXT("CadenceGraphApplication"));
const FText FCadenceGraphApplication::BaseToolkitName = FText::FromString(TEXT("CadenceGraphApplication"));
const FString FCadenceGraphApplication::WorldCentricTabPrefix = TEXT("CadenceGraphApplication");
const FLinearColor FCadenceGraphApplication::WorldCentricTabColorScale = FLinearColor(0.549f, 0.0f, 0.784f, 0.5f);
const FString FCadenceGraphApplication::DocumentationLink = TEXT("https://github.com/billyfletcher5000");

void FCadenceGraphApplication::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FCadenceGraphApplication::InitEditor(const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost> InToolkitHost, UObject* InObject)
{
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(InObject);

	InitAssetEditor(InMode, InToolkitHost, TEXT("Cadence"), FTabManager::FLayout::NullLayout, true, true, ObjectsToEdit);

	AddApplicationMode(FCadenceGraphApplicationMode::ModeName, MakeShareable(new FCadenceGraphApplicationMode(SharedThis(this))));
	SetCurrentMode(FCadenceGraphApplicationMode::ModeName);
}

void FCadenceGraphApplication::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit)
{
}

void FCadenceGraphApplication::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit)
{
}

const FName FCadenceGraphApplicationMode::ModeName = FName(TEXT("CadenceGraphApplicationMode"));
const FName FCadenceGraphApplicationMode::LayoutName = FName(TEXT("CadenceGraphApplicationMode_Layout_v1"));

FCadenceGraphApplicationMode::FCadenceGraphApplicationMode(TSharedPtr<FCadenceGraphApplication> InApplication)
: FApplicationMode(ModeName)
{
	Application = InApplication;
	Tabs.RegisterFactory(MakeShareable(new FCadenceGraphPrimaryTabFactory(InApplication)));

	TabLayout = FTabManager::NewLayout(LayoutName)
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->AddTab
			(
				FCadenceGraphPrimaryTabFactory::Identifier, ETabState::OpenedTab
			)
		)
	);
}

void FCadenceGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FCadenceGraphApplication> SharedApp = Application.Pin();
	SharedApp->PushTabFactories(Tabs);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FCadenceGraphApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FCadenceGraphApplicationMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

const FName FCadenceGraphPrimaryTabFactory::Identifier = FName(TEXT("CadenceGraphPrimaryTab"));

FCadenceGraphPrimaryTabFactory::FCadenceGraphPrimaryTabFactory(TSharedPtr<FCadenceGraphApplication> InApplication)
: FWorkflowTabFactory(Identifier, InApplication)
{
	Application = InApplication;

	TabLabel = FText::FromString(TEXT("Graph"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the main graph view of a Cadence Graph"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the graph view."));
}

TSharedRef<SWidget> FCadenceGraphPrimaryTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock).Text(FText::FromString(TEXT("This is a test text widget for the primary tab!")));
}

FText FCadenceGraphPrimaryTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A primary view for the graph and whatnot"));
}
