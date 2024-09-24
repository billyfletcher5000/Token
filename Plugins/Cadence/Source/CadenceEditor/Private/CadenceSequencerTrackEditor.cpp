// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSequencerTrackEditor.h"

#include "CadenceEditorModule.h"
#include "CadenceSequencerSectionNameCustomization.h"
#include "SequencerSectionPainter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MVVM/Views/ViewUtilities.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "SequencerTrack/CadenceSequencerTrack.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FCadenceSequencerTrackEditor"

namespace
{
	template<typename T>
	static TArray<FAssetData> DiscoverCustomTypes()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		TArray<FAssetData> ValidClasses;
		{
			FString DesiredClassName = FString::Printf(TEXT("/Script/CoreUObject.Class'%s'"), *T::StaticClass()->GetPathName());

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
		UClass* CustomSequencerTrackClass = T::StaticClass();
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
		int32 LayerIndex = Painter.PaintSectionBackground();

		
		const UCadenceSequencerSection* SectionObject = CastChecked<UCadenceSequencerSection>(Section);

		FString SectionText = SectionObject->GetSectionName();
		if (!SectionText.IsEmpty())
		{
			FSlateClippingZone ClippingZone(Painter.SectionClippingRect.InsetBy(FMargin(1.0f)));

			
			const FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle("Bold", 10, FFontOutlineSettings(1));
			TSharedRef<FSlateFontCache> FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();

			float FontHeight = FontCache->GetMaxCharacterHeight(FontInfo, 1.f) + FontCache->GetBaseline(FontInfo, 1.f);		
			const ESlateDrawEffect DrawEffects = Painter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

			FVector2D TopLeft = Painter.SectionGeometry.AbsoluteToLocal(Painter.SectionClippingRect.GetTopLeft()) + FVector2D(1.f, -1.f);
			FMargin ContentPadding(2.0f, 10.0f);
			
			Painter.DrawElements.PushClip(ClippingZone);
			
			FSlateDrawElement::MakeText(
				Painter.DrawElements,
				++LayerIndex,
				Painter.SectionGeometry.ToPaintGeometry(
					FVector2D(Painter.SectionGeometry.Size.X, FontHeight),
					FSlateLayoutTransform(TopLeft + FVector2D(ContentPadding.Left, ContentPadding.Top))
				),
				SectionText,
				FontInfo,
				DrawEffects,
				FColor(200, 200, 200, static_cast<uint8>(255 * Painter.GhostAlpha))
			);

			Painter.DrawElements.PopClip();
		}
		
		return LayerIndex;
	}
	
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override
	{
		Collector.AddReferencedObject(Section);
	}
	
	virtual FString GetReferencerName() const override
	{
		return TEXT("FCustomSection");
	}

	virtual void CustomizePropertiesDetailsView(TSharedRef<IDetailsView> DetailsView, const FSequencerSectionPropertyDetailsViewCustomizationParams& InParams) const override
	{
		UCadenceSequencerSection* CadenceSection = CastChecked<UCadenceSequencerSection>(Section);
		
		DetailsView->RegisterInstancedCustomPropertyTypeLayout(FCadenceSectionName::StaticStruct()->GetFName(),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FCadenceSequencerSectionNameEditCustomization::MakeInstance, CadenceSection->GetParentTrack() ) );
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

		TArray<FAssetData> SectionTypes = DiscoverCustomTypes<UCadenceSequencerSection>();

		for (const FAssetData& SectionAsset : SectionTypes)
		{			
			UClass* Class = LoadClassFromAssetData(SectionAsset);

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
	TArray<FAssetData> TrackTypes = DiscoverCustomTypes<UCadenceSequencerTrack>();

	for (const FAssetData& Asset : TrackTypes)
	{
		UClass* ThisClass = LoadClassFromAssetData(Asset);
		if (!ThisClass)
		{
			continue;
		}

		UCadenceSequencerTrack* TrackCDO = Cast<UCadenceSequencerTrack>(ThisClass->ClassDefaultObject);
		if(!TrackCDO)
		{
			continue;
		}

		MenuBuilder.AddMenuEntry(
			TrackCDO->TrackName,
			FText(),
			FSlateIcon(FCadenceEditorModule::StyleSetName, TEXT("ClassIcon.CadenceTrack")),
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

const FSlateBrush* FCadenceSequencerTrackEditor::GetIconBrush() const
{
	const ISlateStyle* SlateStyle = FSlateStyleRegistry::FindSlateStyle(FCadenceEditorModule::StyleSetName);	
	return SlateStyle->GetBrush("ClassIcon.CadenceTrack");
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
	CustomTrack->SetDisplayName(FText::FromString("Cadence Track"));
	
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
	constexpr float SectionDefaultDuration = 5.0f;
	
	TSharedPtr<ISequencer> SequencerPin = GetSequencer();
	UClass*                Class     = ClassType.Get();

	if (Class && SequencerPin)
	{
		FScopedTransaction Transaction(FText::Format(FText::FromString(TEXT("Add New Section From Class {0}")), FText::FromName(Class->GetFName())));

		Track->Modify();
			
		UCadenceSequencerSection* NewSection = NewObject<UCadenceSequencerSection>(Track, Class, NAME_None, RF_Transactional);

		NewSection->SetParentTrack(Track);
		NewSection->SetSectionName(GetUniqueSectionName(Track, NewSection->GetSectionName()));

		FQualifiedFrameTime CurrentTime = SequencerPin->GetLocalTime();

		const FFrameNumber Duration = (SectionDefaultDuration * CurrentTime.Rate).FrameNumber;
		NewSection->SetRange(TRange<FFrameNumber>(CurrentTime.Time.FrameNumber, CurrentTime.Time.FrameNumber + Duration));
		NewSection->InitialPlacement(Track->GetAllSections(), CurrentTime.Time.FrameNumber, Duration.Value, Track->SupportsMultipleRows());

		if(NewSection->GetColorTint() == FColor::Transparent)
		{
			int32 NumSections = Track->GetAllSections().Num();
			NewSection->SetColorTint(GetRandomSectionColor(NumSections));
		}

		Track->AddSection(*NewSection);

		SequencerPin->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
	}
}

FString FCadenceSequencerTrackEditor::GetUniqueSectionName(UCadenceSequencerTrack* Track, const FString& IntendedName)
{
	TArray<UCadenceSequencerSection*> PotentiallyConflictedSections;

	bool bAnyDirectConflict = false;
	for(UMovieSceneSection* MovieSection : Track->GetAllSections())
	{
		UCadenceSequencerSection* Section = CastChecked<UCadenceSequencerSection>(MovieSection);
		FString SectionName = Section->GetSectionName();
		
		if(SectionName.Contains(IntendedName))
		{
			PotentiallyConflictedSections.Add(Section);
			
			if(SectionName.Equals(IntendedName, ESearchCase::IgnoreCase))
				bAnyDirectConflict = true;
		}
	}

	if(!bAnyDirectConflict)
		return IntendedName;

	int32 SuffixIndex = 2; // Start at 2 so it makes logical sense, e.g. 'Rise', 'Rise 2'
	constexpr int32 MaxAttempts = 99;
	while(SuffixIndex <= MaxAttempts)
	{
		FString TestName = IntendedName + FString(" ") + FString::FromInt(SuffixIndex);

		bAnyDirectConflict = false;
		for(UCadenceSequencerSection* Section : PotentiallyConflictedSections)
		{
			FString SectionName = Section->GetSectionName();
			if(SectionName.Equals(TestName, ESearchCase::IgnoreCase))
			{
				bAnyDirectConflict = true;
				break;
			}
		}

		if(!bAnyDirectConflict)
			return TestName;

		SuffixIndex++;
	}

	return IntendedName + FString(" ") + FGuid::NewGuid().ToString();
}

FColor FCadenceSequencerTrackEditor::GetRandomSectionColor(const int32& InSectionIndex)
{
	constexpr float SectionHueStride = 20.0f;

	float NumRepeats = FMath::Floor((InSectionIndex * SectionHueStride) / 255.0f);
	float InitialOffset = NumRepeats > 0.0f ? SectionHueStride / (NumRepeats + 1.0f) : 0.0f;

	int32 Hue = FMath::FloorToInt32((InSectionIndex * SectionHueStride) + InitialOffset);
	return FLinearColor::MakeFromHSV8(Hue, 255, 255).ToFColor(true);
}
