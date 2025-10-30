// Fill out your copyright notice in the Description page of Project Settings.


#include "FCadenceRhythmVisualiser.h"

#include "CadenceGraphPropertyCustomization.h"
#include "ISequencerModule.h"
#include "MovieScene.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Sections/MovieSceneAudioSection.h"
#include "Tracks/MovieSceneAudioTrack.h"

#define LOCTEXT_NAMESPACE "FCadenceRhythmVisualiser"

void FCadenceRhythmVisualiser::RegisterSequencerExtensions()
{	
    UE_LOG(LogCadenceEditor, Warning, TEXT("Registering Sequencer menu extensions via ToolMenus"));
    
    FToolMenuOwnerScoped OwnerScoped(this);
    
    // To find menu names, use console command: ToolMenus.Edit 1
    // Then right-click in Sequencer to see the menu path
    
    // Extend the Sequencer outliner (track list) context menu
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("SequenceNavigator.ToolBar");
        if (Menu)
        {
            UE_LOG(LogCadenceEditor, Warning, TEXT("Successfully extended Sequencer.OutlinerContextMenu"));
            
            FToolMenuSection& Section = Menu->AddSection(
                "AudioAnalysis",
                LOCTEXT("AudioAnalysis", "Audio Analysis"),
                FToolMenuInsert("Edit", EToolMenuInsertType::After)
            );
            
            Section.AddDynamicEntry("AudioAnalysisEntry", FNewToolMenuSectionDelegate::CreateLambda(
                [](FToolMenuSection& InSection)
                {
                    UE_LOG(LogCadenceEditor, Warning, TEXT("Building Audio Analysis menu section"));

                    // Add menu entry
                    FToolMenuEntry& Entry = InSection.AddMenuEntry(
                        "DetectTransients",
                        LOCTEXT("DetectTransients", "Detect Transients"),
                        LOCTEXT("DetectTransientsTooltip", "Analyze audio and detect transient peaks"),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateLambda([]()
                            {
                                UE_LOG(LogCadenceEditor, Warning, TEXT("Detect Transients clicked!"));
                                
                                // Note: Getting context here is tricky
                                // You may need to store selected tracks in a separate system
                                // or use ISequencer interface to get selection
                            }),
                            FCanExecuteAction::CreateLambda([]() { return true; })
                        )
                    );
                    
                    InSection.AddMenuEntry(
                        "CreateMarkersFromTransients",
                        LOCTEXT("CreateMarkers", "Create Markers from Transients"),
                        LOCTEXT("CreateMarkersTooltip", "Detect transients and create markers"),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateLambda([]()
                            {
                                UE_LOG(LogCadenceEditor, Warning, TEXT("Create Markers clicked!"));
                            })
                        )
                    );
                }
            ));
        }
        else
        {
            UE_LOG(LogCadenceEditor, Error, TEXT("Failed to find Sequencer.OutlinerContextMenu"));
        }
    }
    
    // You can also try these other menu paths:
    // "Sequencer.TracksMenu" - Track area context menu
    // "Sequencer.SectionContextMenu" - Audio section context menu
    
    // Extend section context menu (for right-clicking on audio sections)
    {
        UToolMenu* SectionMenu = UToolMenus::Get()->ExtendMenu("Sequencer.MainToolBar");
        if (SectionMenu)
        {
            UE_LOG(LogCadenceEditor, Warning, TEXT("Successfully extended Sequencer.TracksMenu"));
            
            FToolMenuSection& SectionMenuSection = SectionMenu->AddSection(
                "AudioAnalysisSection",
                LOCTEXT("AudioAnalysisSection", "Audio Analysis")
            );
            
            SectionMenuSection.AddMenuEntry(
                "DetectTransients",
                FText::GetEmpty(),
                LOCTEXT("DetectTransientsSectionTooltip", "Analyze selected audio track or section for transients"),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([]()
                    {
                        UE_LOG(LogCadenceEditor, Warning, TEXT("Detect Transients (Section) clicked!"));
                    }),
                    FCanExecuteAction::CreateLambda([]() { return true; })
                )
            );
        }
    }
}


void FCadenceRhythmVisualiser::UnregisterSequencerExtensions()
{
	if (UObjectInitialized() && !IsEngineExitRequested())
	{
		UToolMenus::UnregisterOwner(this);
	}
}

void FCadenceRhythmVisualiser::OnSequencerCreated(TSharedRef<ISequencer> InSequencer)
{
    ActiveSequencers.Add(InSequencer);
    UE_LOG(LogCadenceEditor, Log, TEXT("Stored sequencer reference"));
}

void FCadenceRhythmVisualiser::RegisterMenuExtensions()
{
    // Register sequencer creation callback
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
    SequencerModule.RegisterOnSequencerCreated(
        FOnSequencerCreated::FDelegate::CreateRaw(this, &FCadenceRhythmVisualiser::OnSequencerCreated)
    );
    
    // Register ToolMenu
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateLambda([this]()
        {
            FToolMenuOwnerScoped OwnerScoped(this);
            
            if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("Sequencer.MainToolBar"))
            {
                FToolMenuSection& Section = Menu->AddSection(
                    "AudioAnalysis",
                    FText::FromString("Audio Analysis")
                );
                
                Section.AddMenuEntry(
                    "DetectTransients",
                    FText::GetEmpty(),
                    FText::FromString("Analyze selected audio tracks for transients"),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([this]()
                        {
                            // Get the active sequencer and query its selection
                            TSharedPtr<ISequencer> ActiveSequencer = GetActiveSequencer();
                            if (!ActiveSequencer.IsValid())
                            {
                                UE_LOG(LogCadenceEditor, Warning, TEXT("No active sequencer found"));
                                return;
                            }
                            
                            // Get selected tracks
                            TArray<UMovieSceneTrack*> SelectedTracks;
                            ActiveSequencer->GetSelectedTracks(SelectedTracks);
                            
                            UE_LOG(LogCadenceEditor, Log, TEXT("Found %d selected tracks"), SelectedTracks.Num());
                            
                            for (UMovieSceneTrack* Track : SelectedTracks)
                            {
                                if (UMovieSceneAudioTrack* AudioTrack = Cast<UMovieSceneAudioTrack>(Track))
                                {
                                    UE_LOG(LogCadenceEditor, Log, TEXT("Selected Audio Track: %s"), 
                                        *AudioTrack->GetDisplayName().ToString());
                                    
                                    // Process the track
                                    ProcessAudioTrack(AudioTrack, ActiveSequencer);
                                }
                            }
                            
                            // Also get selected sections
                            TArray<UMovieSceneSection*> SelectedSections;
                            ActiveSequencer->GetSelectedSections(SelectedSections);
                            
                            UE_LOG(LogCadenceEditor, Log, TEXT("Found %d selected sections"), SelectedSections.Num());
                            
                            for (UMovieSceneSection* Section : SelectedSections)
                            {
                                if (UMovieSceneAudioSection* AudioSection = Cast<UMovieSceneAudioSection>(Section))
                                {
                                    if (USoundWave* SoundWave = Cast<USoundWave>(AudioSection->GetSound()))
                                    {
                                        UE_LOG(LogCadenceEditor, Log, TEXT("Selected Audio Section: %s"), 
                                            *SoundWave->GetName());
                                        
                                        // Process the section
                                        ProcessAudioSection(AudioSection, ActiveSequencer);
                                    }
                                }
                            }
                        }),
                        FCanExecuteAction::CreateLambda([this]()
                        {
                            // Enable only if there's an active sequencer with audio tracks selected
                            TSharedPtr<ISequencer> ActiveSequencer = GetActiveSequencer();
                            if (!ActiveSequencer.IsValid())
                            {
                                return false;
                            }
                            
                            TArray<UMovieSceneTrack*> SelectedTracks;
                            ActiveSequencer->GetSelectedTracks(SelectedTracks);
                            
                            // Check if any selected track is an audio track
                            for (UMovieSceneTrack* Track : SelectedTracks)
                            {
                                if (Cast<UMovieSceneAudioTrack>(Track))
                                {
                                    return true;
                                }
                            }
                            
                            // Also check sections
                            TArray<UMovieSceneSection*> SelectedSections;
                            ActiveSequencer->GetSelectedSections(SelectedSections);
                            
                            for (UMovieSceneSection* Section : SelectedSections)
                            {
                                if (Cast<UMovieSceneAudioSection>(Section))
                                {
                                    return true;
                                }
                            }
                            
                            return false;
                        })
                    )
                );

            	Section.AddSubMenu(
					"SelectTransientSettingsSubMenu", 
					TAttribute<FText>::CreateLambda([this]
					{
						return CurrentTransientDetectorSettings.IsValid()
								? FText::FromString(CurrentTransientDetectorSettings.Get()->GetName())
								: LOCTEXT("SelectTransientSettingsSubMenuLabel", "Select Transient Settings");
					}),					
					LOCTEXT("SelectTransientSettingsSubMenuToolTip", "Select Transient Settings asset used for calculating transients"),
					FNewToolMenuDelegate::CreateRaw(this, &FCadenceRhythmVisualiser::MakeSelectTransientSettingsSubMenu),
					FUIAction(
						FExecuteAction(),
						FCanExecuteAction::CreateLambda([] { return true; })
						),
					EUserInterfaceActionType::Button,
					false, 
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Adjust")
				);
            }
        })
    );
}

void FCadenceRhythmVisualiser::MakeSelectTransientSettingsSubMenu(UToolMenu* ToolMenu)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	const UClass* Class = UCadenceTransientDetectorSettings::StaticClass();
	AssetRegistryModule.Get().GetAssetsByClass(Class->GetClassPathName(), AssetData);

	
	FToolMenuSection& Section = ToolMenu->AddSection("SelectTransientSettingsSection");

	
	for (FAssetData& Data : AssetData)
	{
		Section.AddMenuEntry(					
					Data.AssetName,					
					FText::FromName(Data.AssetName),					
					FText::FromName(Data.AssetName),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([this, Data]
						{
							UCadenceTransientDetectorSettings* NewSettings = Cast<UCadenceTransientDetectorSettings>(Data.GetAsset());
							if (IsValid(NewSettings))
							{
								CurrentTransientDetectorSettings = NewSettings;
							}
						}),
						FCanExecuteAction::CreateLambda([] { return true; })
						)
				);
	}
}

TSharedPtr<ISequencer> FCadenceRhythmVisualiser::GetActiveSequencer()
{
    // Clean up invalid weak pointers
    ActiveSequencers.RemoveAll([](const TWeakPtr<ISequencer>& Ptr) { return !Ptr.IsValid(); });
    
    // Return the first valid sequencer (could be improved to find the focused one)
    for (const TWeakPtr<ISequencer>& WeakSequencer : ActiveSequencers)
    {
        if (TSharedPtr<ISequencer> Sequencer = WeakSequencer.Pin())
        {
            return Sequencer;
        }
    }
    
    return nullptr;
}

void FCadenceRhythmVisualiser::ProcessAudioTrack(UMovieSceneAudioTrack* AudioTrack, TSharedPtr<ISequencer> Sequencer)
{
    UE_LOG(LogCadenceEditor, Log, TEXT("Processing track: %s"), *AudioTrack->GetDisplayName().ToString());
    
    for (UMovieSceneSection* Section : AudioTrack->GetAllSections())
    {
        if (UMovieSceneAudioSection* AudioSection = Cast<UMovieSceneAudioSection>(Section))
        {
            ProcessAudioSection(AudioSection, Sequencer);
        }
    }
}

void FCadenceRhythmVisualiser::ProcessAudioSection(UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer)
{
    if (USoundWave* SoundWave = Cast<USoundWave>(AudioSection->GetSound()))
    {
        UE_LOG(LogCadenceEditor, Log, TEXT("  Processing section with sound: %s"), *SoundWave->GetName());
        
        // Run your transient detector
        FCadenceTransientDetector Detector;
    	
        if (!CurrentTransientDetectorSettings.IsValid())
        	CurrentTransientDetectorSettings = NewObject<UCadenceTransientDetectorSettings>();
    	
        TArray<FCadenceTransientInfo> Transients = Detector.DetectTransients(SoundWave, CurrentTransientDetectorSettings.Get());
        
        CreateMarkersFromTransients(Transients, AudioSection, Sequencer);
    }
}

void FCadenceRhythmVisualiser::CreateMarkersFromTransients(const TArray<FCadenceTransientInfo>& Transients, UMovieSceneAudioSection* AudioSection, TSharedPtr<ISequencer> Sequencer)
{
	UMovieSceneSequence* Sequence = Sequencer->GetFocusedMovieSceneSequence();
    if (!Sequence)
    {
        return;
    }
    
    UMovieScene* MovieScene = Sequence->GetMovieScene();
    if (!MovieScene)
    {
        return;
    }
	
	MovieScene->DeleteMarkedFrames();
    
    // Get timing information
    FFrameNumber SectionStartFrame = AudioSection->GetInclusiveStartFrame();
    FFrameRate TickResolution = MovieScene->GetTickResolution();
    FFrameRate DisplayRate = MovieScene->GetDisplayRate();
    
    // CRITICAL: Get the start offset (how much of the audio is trimmed from the start)
    FFrameNumber StartOffset = AudioSection->HasStartFrame() ? 
        AudioSection->GetOffsetTime().Get(FFrameTime(0)).FloorToFrame() : 
        FFrameNumber(0);
    
    // Get the sound wave to check its sample rate
    USoundWave* SoundWave = Cast<USoundWave>(AudioSection->GetSound());
    if (!SoundWave)
    {
        UE_LOG(LogCadenceEditor, Warning, TEXT("No SoundWave found in section"));
        return;
    }

	// Verify first transient
	if (Transients.Num() > 0)
	{
		float FirstTransientSeconds = Transients[0].TimeInSeconds;
		FFrameNumber ExpectedFrame = SectionStartFrame + 
			FFrameNumber(FMath::RoundToInt32(FirstTransientSeconds * TickResolution.AsDecimal()));
    
		UE_LOG(LogCadenceEditor, Warning, TEXT("First transient should appear at frame %d (%.2f seconds in timeline)"),
			ExpectedFrame.Value,
			TickResolution.AsSeconds(ExpectedFrame.Value));
	}
    
    float SoundDuration = SoundWave->GetDuration();
    
    // Log debug info
    UE_LOG(LogCadenceEditor, Log, TEXT("Section Start Frame: %d"), SectionStartFrame.Value);
    UE_LOG(LogCadenceEditor, Log, TEXT("Start Offset: %d frames"), StartOffset.Value);
    UE_LOG(LogCadenceEditor, Log, TEXT("Tick Resolution: %f fps"), TickResolution.AsDecimal());
    UE_LOG(LogCadenceEditor, Log, TEXT("Display Rate: %f fps"), DisplayRate.AsDecimal());
    UE_LOG(LogCadenceEditor, Log, TEXT("Sound Duration: %f seconds"), SoundDuration);
    
    // Create a marker for each transient
    for (const FCadenceTransientInfo& Transient : Transients)
    {
        // Convert transient time (in seconds from audio start) to frames
        // Use the tick resolution for accurate timing
        double TransientSeconds = Transient.TimeInSeconds;
        
        // Convert to frame number at the sequence's tick resolution
        FFrameNumber TransientFrameOffset = FFrameNumber(
            FMath::RoundToInt32(TransientSeconds * (TickResolution.AsDecimal() * .5f))
        );
        
        // Calculate final marker position:
        // Section start + transient offset - any start offset trim
        FFrameNumber MarkerFrame = SectionStartFrame + TransientFrameOffset - StartOffset;
        
        // Clamp to section bounds
        FFrameNumber SectionEndFrame = AudioSection->GetExclusiveEndFrame();
        if (MarkerFrame < SectionStartFrame || MarkerFrame >= SectionEndFrame)
        {
            UE_LOG(LogCadenceEditor, Warning, TEXT("Transient at %.3fs (frame %d) is outside section bounds [%d, %d)"), 
                TransientSeconds, MarkerFrame.Value, SectionStartFrame.Value, SectionEndFrame.Value);
            continue;
        }
        
        // Add marker to the movie scene
        FMovieSceneMarkedFrame Marker(MarkerFrame);
        
        // Set marker label with timing info
        Marker.Label = FString::Printf(TEXT("T:%.2fs [%.2f]"), 
            TransientSeconds, Transient.TransientStrength);
    	
    	MovieScene->AddMarkedFrame(Marker);
        
        // Convert to display rate for logging
        double MarkerTimeSeconds = TickResolution.AsSeconds(MarkerFrame.Value);
        
        UE_LOG(LogCadenceEditor, Log, TEXT("Created marker at frame %d (%.3f seconds) for transient at %.3fs in audio"), 
            MarkerFrame.Value, MarkerTimeSeconds, TransientSeconds);
    }
    
    // Notify sequencer of changes
    Sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemsChanged); 
    
    UE_LOG(LogCadenceEditor, Log, TEXT("Created %d markers"), Transients.Num());
}

#undef LOCTEXT_NAMESPACE