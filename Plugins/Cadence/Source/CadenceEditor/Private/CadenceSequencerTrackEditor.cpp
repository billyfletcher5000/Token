// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSequencerTrackEditor.h"

#include "CadenceSequencerTracksStyle.h"
#include "SequencerSectionPainter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MVVM/Views/ViewUtilities.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "SequencerTrack/CadenceSequencerTrack.h"

#define LOCTEXT_NAMESPACE "FCadenceSequencerTrackEditor"

namespace
{
	static TArray<FAssetData> DiscoverCustomTrackTypes()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		TArray<FAssetData> ValidClasses;
		{
			FString DesiredClassName = FString::Printf(TEXT("/Script/CoreUObject.Class'%s'"), *UCadenceSequencerTrack::StaticClass()->GetPathName());

			FARFilter Filter;
			Filter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, MoveTemp(DesiredClassName));
			AssetRegistryModule.Get().GetAssets(Filter, ValidClasses);
		}

		TSet<FName> ExistingPaths;
		for (const FAssetData& Asset : ValidClasses)
		{
			FAssetDataTagMapSharedView::FFindTagResult GeneratedClassPathTag = Asset.TagsAndValues.FindTag(FBlueprintTags::GeneratedClassPath);
			if (GeneratedClassPathTag.IsSet())
			{
				FString ObjectPath = FPackageName::ExportTextPathToObjectPath(GeneratedClassPathTag.GetValue());
				ExistingPaths.Add(*ObjectPath);
			}
		}

		// Check loaded classes
		UClass* CustomSequencerTrackClass = UCadenceSequencerTrack::StaticClass();
		for (TObjectIterator<UClass> ClassIterator; ClassIterator; ++ClassIterator)
		{
			UClass* Class = *ClassIterator;
			if (Class == CustomSequencerTrackClass || (Class->IsChildOf(CustomSequencerTrackClass) && !Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists) && Class->GetAuthoritativeClass() == Class))
			{
				if (!ExistingPaths.Contains(*Class->GetPathName()))
				{
					ValidClasses.Add(*ClassIterator);
				}
			}
		}

		return ValidClasses;
	}
	
	UClass* LoadClassFromAssetData(const FAssetData& AssetData)
	{
		UObject* LoadedAsset = AssetData.FastGetAsset();
		if (!LoadedAsset)
		{
			FScopedSlowTask SlowTask(1.f, FText::Format(FText::FromString(TEXT("Loading asset {0}")), FText::FromName(AssetData.AssetName)));
			SlowTask.MakeDialogDelayed(1.f);

			LoadedAsset = AssetData.GetAsset();
		}

		if (!LoadedAsset)
		{
			return nullptr;
		}

		if (UClass* DirectClass = Cast<UClass>(LoadedAsset))
		{
			return DirectClass;
		}
		else if (UBlueprint* Blueprint = Cast<UBlueprint>(LoadedAsset))
		{
			return Blueprint->GeneratedClass;
		}
		return nullptr;
	}
}

struct FCadenceCustomSection : public ISequencerSection, public FGCObject
{
	FCadenceCustomSection(UMovieSceneSection* InSection)
		: Section(InSection)
	{}

	virtual UMovieSceneSection* GetSectionObject() override
	{
		return Section;
	}

	virtual int32 OnPaintSection(FSequencerSectionPainter& Painter) const override
	{
		return Painter.PaintSectionBackground();
	}

	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override
	{
		Collector.AddReferencedObject(Section);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCustomSection");
	}

	TObjectPtr<UMovieSceneSection> Section;
};

FCadenceSequencerTrackEditor::FCadenceSequencerTrackEditor(TSharedRef<ISequencer> InSequencer)
: FMovieSceneTrackEditor(InSequencer)
{
}

TSharedPtr<SWidget> FCadenceSequencerTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
	UCadenceSequencerTrack* CustomTrack = Cast<UCadenceSequencerTrack>(Track);
	if (!CustomTrack)
	{
		return nullptr;
	}

	auto SubMenuCallback = [this, CustomTrack]() -> TSharedRef<SWidget>
	{
		FMenuBuilder MenuBuilder(true, nullptr);

		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* Class = *It;

			if (Class->IsChildOf(UCadenceSequencerSection::StaticClass()) &&
				!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_Hidden))
			{
				this->MakeMenuEntry(MenuBuilder, CustomTrack, Class);
			}
		}

		return MenuBuilder.MakeWidget();
	};

	return UE::Sequencer::MakeAddButton(FText::FromString(TEXT("Section")), FOnGetContent::CreateLambda(SubMenuCallback), Params.ViewModel);
}

TSharedRef<ISequencerSection> FCadenceSequencerTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject,	UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	return MakeShared<FCadenceCustomSection>(&SectionObject);
}

void FCadenceSequencerTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	FCadenceSequencerTracksStyle& StyleSet = FCadenceSequencerTracksStyle::Get();

	TArray<FAssetData> TrackTypes = DiscoverCustomTrackTypes();

	for (const FAssetData& Asset : TrackTypes)
	{
		UClass* ThisClass = LoadClassFromAssetData(Asset);
		if (!ThisClass)
		{
			continue;
		}

		StyleSet.RegisterNewTrackType(ThisClass);

		MenuBuilder.AddMenuEntry(
			FText::FromName(Asset.AssetName),
			FText(),
			FSlateIcon(StyleSet.GetStyleSetName(), ThisClass->GetFName()),
			FUIAction(
				FExecuteAction::CreateSP(this, &FCadenceSequencerTrackEditor::AddNewTrack, Asset)
			)
		);
	}
}

bool FCadenceSequencerTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
	return Type->IsChildOf(UCadenceSequencerTrack::StaticClass());
}

void FCadenceSequencerTrackEditor::AddNewTrack(FAssetData AssetData)
{
	UMovieScene* MovieScene = GetFocusedMovieScene();
	if (MovieScene == nullptr || MovieScene->IsReadOnly())
	{
		return;
	}

	UClass* ClassToAdd = LoadClassFromAssetData(AssetData);
	if (!ensure(ClassToAdd && ClassToAdd->IsChildOf(UCadenceSequencerTrack::StaticClass())))
	{
		// @todo: notification error
		return;
	}

	const FScopedTransaction Transaction(FText::Format(FText::FromString(TEXT("Add Track {0}")), FText::FromName(ClassToAdd->GetFName())));

	MovieScene->Modify();

	UCadenceSequencerTrack* CustomTrack = CastChecked<UCadenceSequencerTrack>(MovieScene->AddTrack(ClassToAdd));

	if (GetSequencer().IsValid())
	{
		GetSequencer()->OnAddTrack(CustomTrack, FGuid());
	}
}

void FCadenceSequencerTrackEditor::AddNewObjectBindingTrack(FAssetData AssetData, TArray<FGuid> InObjectBindings)
{
	UMovieScene* MovieScene = GetFocusedMovieScene();
	if (MovieScene == nullptr || MovieScene->IsReadOnly())
	{
		return;
	}

	UClass* ClassToAdd = LoadClassFromAssetData(AssetData);
	if (!ensure(ClassToAdd && ClassToAdd->IsChildOf(UCadenceSequencerTrack::StaticClass())))
	{
		// @todo: notification error
		return;
	}

	const FScopedTransaction Transaction(FText::Format(FText::FromString(TEXT("Add Cadence Track {0}")), FText::FromName(ClassToAdd->GetFName())));

	MovieScene->Modify();

	for (const FGuid& ObjectBindingID : InObjectBindings)
	{
		UCadenceSequencerTrack* CustomTrack = CastChecked<UCadenceSequencerTrack>(MovieScene->AddTrack(ClassToAdd, ObjectBindingID));

		if (GetSequencer().IsValid())
		{
			GetSequencer()->OnAddTrack(CustomTrack, FGuid());
		}
	}
}

void FCadenceSequencerTrackEditor::MakeMenuEntry(FMenuBuilder& MenuBuilder, UCadenceSequencerTrack* Track, TSubclassOf<UCadenceSequencerSection> ClassType)
{	
	UClass* Class = ClassType.Get();
	if (Class)
	{
		MenuBuilder.AddMenuEntry(Class->GetDisplayNameText(), Class->GetToolTipText(), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &FCadenceSequencerTrackEditor::CreateNewSection, Track, ClassType)));
	}
}

void FCadenceSequencerTrackEditor::CreateNewSection(UCadenceSequencerTrack* Track, TSubclassOf<UCadenceSequencerSection> ClassType)
{
	TSharedPtr<ISequencer> SequencerPin = GetSequencer();
	UClass*                Class     = ClassType.Get();

	if (Class && SequencerPin)
	{
		FScopedTransaction Transaction(FText::Format(FText::FromString(TEXT("Add New Section From Class {0}")), FText::FromName(Class->GetFName())));

		Track->Modify();
			
		UCadenceSequencerSection* NewSection = NewObject<UCadenceSequencerSection>(Track, Class, NAME_None, RF_Transactional);

		FQualifiedFrameTime CurrentTime = SequencerPin->GetLocalTime();

		const FFrameNumber Duration = (5.f * CurrentTime.Rate).FrameNumber;
		NewSection->SetRange(TRange<FFrameNumber>(CurrentTime.Time.FrameNumber, CurrentTime.Time.FrameNumber + Duration));
		NewSection->InitialPlacement(Track->GetAllSections(), CurrentTime.Time.FrameNumber, Duration.Value, Track->SupportsMultipleRows());

		Track->AddSection(*NewSection);

		SequencerPin->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
	}
}
