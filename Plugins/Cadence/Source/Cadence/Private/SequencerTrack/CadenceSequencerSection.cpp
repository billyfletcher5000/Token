// Fill out your copyright notice in the Description page of Project Settings.


#include "SequencerTrack/CadenceSequencerSection.h"

#include "EntitySystem/BuiltInComponentTypes.h"
#include "SequencerTrack/CadenceSequencerTrack.h"
#include "SequencerTrack/CadenceSequencerTrackInstance.h"

UCadenceSequencerSection::UCadenceSequencerSection(const FObjectInitializer& ObjInit)
: Super(ObjInit)
{
	// This section must be public as the object animator system needs to reference it and it lives in a different package.
	// Without this flag, object reinstancing will clear out the pointer to the section with FArchiveReplaceOrClearExternalReferences
	SetFlags(RF_Public);
}

void UCadenceSequencerSection::ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker,
	const FEntityImportParams& Params, FImportedEntity* OutImportedEntity)
{
	using namespace UE::MovieScene;
	
	FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();

	FGuid ObjectBindingID = Params.GetObjectBindingID();
	OutImportedEntity->AddBuilder(
		FEntityBuilder()
		.Add(BuiltInComponents->TrackInstance, FMovieSceneTrackInstanceComponent{ decltype(FMovieSceneTrackInstanceComponent::Owner)(this), UCadenceSequencerTrackInstance::StaticClass() })
		.AddConditional(BuiltInComponents->GenericObjectBinding, ObjectBindingID, ObjectBindingID.IsValid())
		.AddTagConditional(BuiltInComponents->Tags.Root, !ObjectBindingID.IsValid())
	);	
}
