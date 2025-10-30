// Copyright (C) Billy Fletcher 2025


#include "CadenceSequenceActorCustomization.h"

#include "DetailLayoutBuilder.h"
#include "SequencerTrack/CadenceSequenceActor.h"

TSharedRef<IDetailCustomization> FCadenceSequenceActorCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceSequenceActorCustomization);
}

void FCadenceSequenceActorCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(ACadenceSequenceActor, LevelSequenceAsset));
}
