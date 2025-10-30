// Fill out your copyright notice in the Description page of Project Settings.

#include "Utility/CadenceTransientDetector.h"

#include "CadenceGraphPropertyCustomization.h"

FCadenceTransientDetector::FCadenceTransientDetector()
{
}

// Envelope Follower Implementation
FCadenceTransientDetector::FEnvelopeFollower::FEnvelopeFollower(const float InAttackTime, const float InReleaseTime, const float InSampleRate)
: CurrentEnvelope(0.0f)
{
    // Convert time constants to coefficients
    // Using exponential smoothing: coeff = exp(-1/(time * sampleRate))
    AttackCoefficient = FMath::Exp(-1.0f / (InAttackTime * InSampleRate));
    ReleaseCoefficient = FMath::Exp(-1.0f / (InReleaseTime * InSampleRate));
}

void FCadenceTransientDetector::FEnvelopeFollower::Reset()
{
    CurrentEnvelope = 0.0f;
}

float FCadenceTransientDetector::FEnvelopeFollower::ProcessSample(const float InInputSample)
{
    // Get absolute value (rectify signal)
    float AbsInput = FMath::Abs(InInputSample);
    
    // Choose attack or release coefficient based on whether signal is rising or falling
    float Coeff = (AbsInput > CurrentEnvelope) ? AttackCoefficient : ReleaseCoefficient;
    
    // Apply one-pole filter: y[n] = coeff * y[n-1] + (1 - coeff) * x[n]
    CurrentEnvelope = Coeff * CurrentEnvelope + (1.0f - Coeff) * AbsInput;
    
    return CurrentEnvelope;
}

bool FCadenceTransientDetector::GetPCMData(USoundWave* InSoundWave, TArray<float>& OutPCMData)
{
    if (!InSoundWave)
    {
        return false;
    }

#if WITH_EDITORONLY_DATA
	if (InSoundWave->RawData.GetPayloadSize() > 0)
	{
		float duration = InSoundWave->GetDuration();
		UE_LOG(LogCadenceEditor, Log, TEXT("SoundWave Duration: %f"), duration);
		// Create payload for async data retrieval
		FSharedBuffer PayloadBuffer = InSoundWave->RawData.GetPayload().Get();
        
		if (PayloadBuffer.GetSize() > 0)
		{
			const uint8* RawDataPtr = static_cast<const uint8*>(PayloadBuffer.GetData());
			const int32 SampleDataSize = PayloadBuffer.GetSize();
            
			// Assuming 16-bit PCM format
			const int16* SampleData = reinterpret_cast<const int16*>(RawDataPtr);
			const int32 NumSamples = SampleDataSize / sizeof(int16);

			if (NumSamples > 0)
			{
				OutPCMData.SetNum(NumSamples);
				for (int32 i = 0; i < NumSamples; ++i)
				{
					OutPCMData[i] = static_cast<float>(SampleData[i]) / 32768.0f;
				}
                
				UE_LOG(LogCadenceEditor, Log, TEXT("Successfully extracted %d samples from RawData payload"), NumSamples);
				return true;
			}
		}
	}
#endif

    return false;
}

TArray<float> FCadenceTransientDetector::ApplyEnvelopeFollower(const TArray<float>& InAudioData, const float InAttackTime, const float InReleaseTime, const float InSampleRate)
{
    TArray<float> Envelope;
    Envelope.SetNum(InAudioData.Num());
    
    FEnvelopeFollower Follower(InAttackTime, InReleaseTime, InSampleRate);
    
    for (int32 i = 0; i < InAudioData.Num(); ++i)
    {
        Envelope[i] = Follower.ProcessSample(InAudioData[i]);
    }
    
    return Envelope;
}

TArray<float> FCadenceTransientDetector::CalculateDifferentialEnvelope(const TArray<float>& InFastEnvelope, const TArray<float>& InSlowEnvelope)
{
    TArray<float> DiffEnvelope;
    DiffEnvelope.SetNum(InFastEnvelope.Num());
    
    for (int32 i = 0; i < InFastEnvelope.Num(); ++i)
    {
        // Calculate difference and ensure non-negative
        DiffEnvelope[i] = FMath::Max(0.0f, InFastEnvelope[i] - InSlowEnvelope[i]);
    }
    
    return DiffEnvelope;
}

float FCadenceTransientDetector::CalculateAdaptiveThreshold(const TArray<float>& InDiffEnvelope, const float InBaseThreshold)
{
    // Calculate mean and standard deviation
    float Mean = 0.0f;
    for (float Value : InDiffEnvelope)
    {
        Mean += Value;
    }
    Mean /= InDiffEnvelope.Num();
    
    float StdDev = 0.0f;
    for (float Value : InDiffEnvelope)
    {
        float Diff = Value - Mean;
        StdDev += Diff * Diff;
    }
    StdDev = FMath::Sqrt(StdDev / InDiffEnvelope.Num());
    
    // Threshold is mean + (threshold factor * standard deviation)
    float AdaptiveThreshold = Mean + (InBaseThreshold * 10.0f * StdDev);
    
    return AdaptiveThreshold;
}

TArray<int32> FCadenceTransientDetector::FindTransientPeaks(const TArray<float>& InDiffEnvelope, float InThreshold, int32 InMinSamplesBetween)
{
    TArray<int32> Peaks;
    int32 LastPeakIndex = -InMinSamplesBetween;
    
    for (int32 i = 1; i < InDiffEnvelope.Num() - 1; ++i)
    {
        // Check if above threshold
        if (InDiffEnvelope[i] < InThreshold)
        {
            continue;
        }
        
        // Check if local maximum
        if (InDiffEnvelope[i] > InDiffEnvelope[i - 1] &&
            InDiffEnvelope[i] >= InDiffEnvelope[i + 1])
        {
            // Check minimum time between transients
            if (i - LastPeakIndex >= InMinSamplesBetween)
            {
                Peaks.Add(i);
                LastPeakIndex = i;
            }
        }
    }
    
    return Peaks;
}

TArray<FCadenceTransientInfo> FCadenceTransientDetector::DetectTransients(USoundWave* InSoundWave, const UCadenceTransientDetectorSettings* InSettings)
{
    TArray<FCadenceTransientInfo> Transients;

    if (!ensureAlways(InSoundWave))
    {
        UE_LOG(LogCadenceEditor, Warning, TEXT("Invalid SoundWave"));
        return Transients;
    }

    // Get PCM data
    TArray<float> PCMData;
    if (!GetPCMData(InSoundWave, PCMData))
    {
        UE_LOG(LogCadenceEditor, Warning, TEXT("Failed to extract PCM data"));
        return Transients;
    }

    const float SampleRate = InSoundWave->GetSampleRateForCurrentPlatform();
    const int32 MinSamplesBetween = FMath::RoundToInt(InSettings->MinTimeBetweenTransients * SampleRate);

    UE_LOG(LogCadenceEditor, Log, TEXT("Processing %d samples at %.1f Hz"), PCMData.Num(), SampleRate);

    // Apply fast envelope follower
    TArray<float> FastEnvelope = ApplyEnvelopeFollower(
        PCMData,
        InSettings->FastAttackTime,
        InSettings->FastReleaseTime,
        SampleRate
    );

    // Apply slow envelope follower
    TArray<float> SlowEnvelope = ApplyEnvelopeFollower(
        PCMData,
        InSettings->SlowAttackTime,
        InSettings->SlowReleaseTime,
        SampleRate
    );

    // Calculate differential envelope
    TArray<float> DiffEnvelope = CalculateDifferentialEnvelope(FastEnvelope, SlowEnvelope);

    // Calculate threshold
    float DetectionThreshold = InSettings->Threshold;
    if (InSettings->bAdaptiveThreshold)
    {
        DetectionThreshold = CalculateAdaptiveThreshold(DiffEnvelope, InSettings->Threshold);
        UE_LOG(LogCadenceEditor, Log, TEXT("Adaptive threshold: %.4f"), DetectionThreshold);
    }

    // Find transient peaks
    TArray<int32> PeakIndices = FindTransientPeaks(
        DiffEnvelope,
        DetectionThreshold,
        MinSamplesBetween
    );

    // Build transient info array
    for (int32 PeakIdx : PeakIndices)
    {
        FCadenceTransientInfo Info;
        Info.SampleIndex = PeakIdx;
        Info.TimeInSeconds = static_cast<float>(PeakIdx) / SampleRate;
        Info.Amplitude = FastEnvelope[PeakIdx];
        Info.TransientStrength = DiffEnvelope[PeakIdx];
        
        Transients.Add(Info);
    }

    UE_LOG(LogCadenceEditor, Log, TEXT("Detected %d transients"), Transients.Num());
    
    return Transients;
}