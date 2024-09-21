// Fill out your copyright notice in the Description page of Project Settings.


#include "CadenceSequencerSectionNameCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "SequencerTrack/CadenceSequencerSection.h"
#include "SequencerTrack/CadenceSequencerTrack.h"

TSharedRef<IPropertyTypeCustomization> FCadenceSequencerSectionNameCustomization::MakeInstance(UCadenceSequencerTrack* InParentTrack)
{
	return MakeShareable(new FCadenceSequencerSectionNameCustomization(InParentTrack));
}

void FCadenceSequencerSectionNameCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCadenceSequencerSectionNameCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
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
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding( 4.0f, 0.0f)
		.FillWidth(1.0f)
		.VAlign( VAlign_Center )
		[
			SNew(STextBlock)
			.Text(NameHandle->GetPropertyDisplayName())
		]
	]
	.ValueContent()
	[
		TextBox		
	];	
}