// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "CadenceTransientDetector.generated.h"

struct FCadenceTransientInfo
{
    float TimeInSeconds;
    float Amplitude;
    int32 SampleIndex;
    float TransientStrength; // Difference between fast and slow envelopes
};

UCLASS()
class CADENCEEDITOR_API UCadenceTransientDetectorSettings : public UDataAsset
{
	GENERATED_BODY()

public:	
    // Envelope follower attack/release times (in seconds)
	UPROPERTY(EditAnywhere)
    float FastAttackTime = 0.001f;   // 1ms - very responsive
	UPROPERTY(EditAnywhere)
    float FastReleaseTime = 0.010f;  // 10ms
	UPROPERTY(EditAnywhere)
    float SlowAttackTime = 0.050f;   // 50ms - averages signal
	UPROPERTY(EditAnywhere)
    float SlowReleaseTime = 0.100f;  // 100ms
    
    // Detection parameters
	UPROPERTY(EditAnywhere)
    float Threshold = 0.05f;          // Difference threshold (0-1)
	UPROPERTY(EditAnywhere)
    float MinTimeBetweenTransients = 0.050f; // 50ms minimum gap
	UPROPERTY(EditAnywhere)
    bool bAdaptiveThreshold = true;   // Use adaptive threshold based on signal
};

class CADENCEEDITOR_API FCadenceTransientDetector
{
public:
    FCadenceTransientDetector();
    
    // Main detection function using differential envelope
    TArray<FCadenceTransientInfo> DetectTransients(USoundWave* InSoundWave, const UCadenceTransientDetectorSettings* InSettings);

private:
    // Envelope follower class
    class FEnvelopeFollower
    {
    public:
        FEnvelopeFollower(const float InAttackTime, const float InReleaseTime, const float InSampleRate);
        void Reset();
        float ProcessSample(float InInputSample);
        
    private:
        float AttackCoefficient;
        float ReleaseCoefficient;
        float CurrentEnvelope;
    };
    
    // Apply envelope follower to entire buffer
    TArray<float> ApplyEnvelopeFollower(const TArray<float>& InAudioData, const float InAttackTime, const float InReleaseTime, const float InSampleRate);
    
    // Calculate difference between fast and slow envelopes
    TArray<float> CalculateDifferentialEnvelope(const TArray<float>& InFastEnvelope, const TArray<float>& InSlowEnvelope);
    
    // Find peaks in differential envelope
    TArray<int32> FindTransientPeaks(const TArray<float>& InDiffEnvelope, const float InThreshold, const int32 InMinSamplesBetween);
    
    // Extract raw PCM data from SoundWave
    bool GetPCMData(USoundWave* InSoundWave, TArray<float>& OutPCMData);
    
    // Calculate adaptive threshold based on signal statistics
    float CalculateAdaptiveThreshold(const TArray<float>& InDiffEnvelope, const float InBaseThreshold);
};

/*

// Usage Example
void UMyAudioAnalyzer::AnalyzeSoundWithDifferentialEnvelope(USoundWave* SoundWave)
{
    FCadenceTransientDetector Detector;
    
    // Configure settings
    UCadenceTransientDetectorSettings Settings;
    Settings.FastAttackTime = 0.001f;   // 1ms - catches transients quickly
    Settings.FastReleaseTime = 0.010f;  // 10ms
    Settings.SlowAttackTime = 0.050f;   // 50ms - tracks average level
    Settings.SlowReleaseTime = 0.100f;  // 100ms
    Settings.Threshold = 0.05f;         // Sensitivity
    Settings.MinTimeBetweenTransients = 0.050f; // 50ms gap
    Settings.bAdaptiveThreshold = true; // Automatically adjust to signal
    
    TArray<FCadenceTransientInfo> Transients = Detector.DetectTransients(SoundWave, Settings);

    // Process results
    for (const FCadenceTransientInfo& Transient : Transients)
    {
        UE_LOG(LogTemp, Log, 
            TEXT("Transient at %.3fs | Amplitude: %.3f | Strength: %.3f"),
            Transient.TimeInSeconds,
            Transient.Amplitude,
            Transient.TransientStrength);
    }
}

// Preset configurations for different use cases
namespace TransientDetectorPresets
{
    // For detecting strong drum hits
    inline UCadenceTransientDetectorSettings DrumHits()
    {
        UCadenceTransientDetectorSettings Settings = NewObject<UCadenceTransientDetectorSettings>();
        Settings.FastAttackTime = 0.001f;
        Settings.FastReleaseTime = 0.015f;
        Settings.SlowAttackTime = 0.100f;
        Settings.SlowReleaseTime = 0.200f;
        Settings.Threshold = 0.1f;
        Settings.MinTimeBetweenTransients = 0.075f;
        Settings.bAdaptiveThreshold = true;
        return Settings;
    }
    
    // For detecting subtle transients (hi-hats, shakers)
    inline UCadenceTransientDetectorSettings SubtleTransients()
    {
        UCadenceTransientDetectorSettings Settings;
        Settings.FastAttackTime = 0.0005f;
        Settings.FastReleaseTime = 0.005f;
        Settings.SlowAttackTime = 0.030f;
        Settings.SlowReleaseTime = 0.060f;
        Settings.Threshold = 0.02f;
        Settings.MinTimeBetweenTransients = 0.025f;
        Settings.bAdaptiveThreshold = true;
        return Settings;
    }
    
    // For speech onset detection
    inline UCadenceTransientDetectorSettings SpeechOnsets()
    {
        UCadenceTransientDetectorSettings Settings;
        Settings.FastAttackTime = 0.005f;
        Settings.FastReleaseTime = 0.020f;
        Settings.SlowAttackTime = 0.100f;
        Settings.SlowReleaseTime = 0.150f;
        Settings.Threshold = 0.08f;
        Settings.MinTimeBetweenTransients = 0.150f;
        Settings.bAdaptiveThreshold = true;
        return Settings;
    }
}*/