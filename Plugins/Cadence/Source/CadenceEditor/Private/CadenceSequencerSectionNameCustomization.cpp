// Copyright (C) Billy Fletcher 2025


#include "CadenceSequencerSectionNameCustomization.h"

#include "CadenceGraphPropertyCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Graph/CadenceGraph.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "SequencerTrack/CadenceSequencerTrack.h"

TSharedRef<IPropertyTypeCustomization> FCadenceSequencerSectionNameEditCustomization::MakeInstance(UCadenceSequencerTrack* InParentTrack)
{
	return MakeShareable(new FCadenceSequencerSectionNameEditCustomization(InParentTrack));
}

void FCadenceSequencerSectionNameEditCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceSequencerSectionNameEditCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	NameHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCadenceSectionName, Name));
	
	if(NameHandle->GetValue(CurrentValue) != FPropertyAccess::Result::Success)
		return;	
	
	auto TextBox = SNew(SHorizontalBox);
	
	TextBox->AddSlot()		
	.MaxWidth(16.0f)
	.AutoWidth()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()		
		[
			SNew(SImage)
			.Visibility_Lambda([&]()
			{
				return ParentTrack->HasMultipleSectionsWithName(CurrentValue) ? EVisibility::Visible : EVisibility::Collapsed;					
			})
			.Image(FCoreStyle::Get().GetBrush("Icons.Warning"))
			.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
			.ToolTip(FSlateApplicationBase::Get().MakeToolTip(FText::FromString(TEXT("Section with same name already exists! As triggers are created by name this could make this section un-referencable!"))))
		]
	];	

	TextBox->AddSlot()
	.FillWidth(1.0f)
	[
		SNew(SEditableTextBox)
		.Text_Lambda([&]()
		{
			return FText::FromString(CurrentValue);
		})
		.OnTextChanged_Lambda([&](const FText& InText)
		{
			CurrentValue = InText.ToString();
			NameHandle->SetValue(CurrentValue);
		})
	];

	ChildBuilder.AddCustomRow(FText::FromString(TEXT("NameRow")))
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		TextBox		
	];	
}

TSharedRef<IPropertyTypeCustomization> FCadenceSequencerSectionNameSelectCustomization::MakeInstance(UCadenceGraph* InGraph)
{
	return MakeShareable(new FCadenceSequencerSectionNameSelectCustomization(InGraph));
}

void FCadenceSequencerSectionNameSelectCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceSequencerSectionNameSelectCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	NameHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCadenceSectionName, Name));
	FString CurrentValueName;
	NameHandle->GetValue(CurrentValueName);
	
	Sections = ParentGraph->GetSections();
	if(Sections.Num() == 0)
	{
		ChildBuilder.AddCustomRow(FText::FromString(TEXT("NameRow")))
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("No sections available!")))
		];

		return;
	}

	TArray<FString> SectionStrings;
	
	UCadenceSequencerSection* CurrentSection = nullptr;
	
	for(UCadenceSequencerSection* Section : Sections)
	{
		if(CurrentValueName == Section->GetSectionName())
		{
			CurrentSection = Section;
			break;
		}
	}

	if(CurrentSection == nullptr)
	{
		if(!CurrentValueName.IsEmpty())
			UE_LOG(LogCadenceEditor, Warning, TEXT("Node has unrecognised section ID: %s"), *CurrentValueName);

		CurrentSection = Sections[0];
	}

	auto MakeWidgetForOption = [](UCadenceSequencerSection* InOption)->TSharedRef<SWidget>
	{
		return SNew(STextBlock).Text(FText::FromString(InOption->GetSectionName()));
	};

	auto OnSelectionChanged = [&](UCadenceSequencerSection* Selection, ESelectInfo::Type SelectType)
	{
		if(IsValid(Selection))
			NameHandle->SetValue(Selection->GetSectionName());
	};

	auto GetValue = [&]()->FText
	{
		FString ValueStr;
		NameHandle->GetValue(ValueStr);
		return FText::FromString(ValueStr);
	};

	ChildBuilder.AddCustomRow(FText::FromString(TEXT("NameRow")))
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SComboBox<UCadenceSequencerSection*>)
		.OptionsSource(&Sections)
		.OnSelectionChanged_Lambda(OnSelectionChanged)
		.OnGenerateWidget_Lambda(MakeWidgetForOption)
		.InitiallySelectedItem(CurrentSection)
		[
			SNew(STextBlock)
			.Text_Lambda(GetValue)
		]
	];
}
