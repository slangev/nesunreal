// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesPulse.h"
#include <memory>

#include "FNesAudioMixer.h"
#include "Components/SynthComponent.h"
#include "DSP/Osc.h"
#include "NesApuSoundOutput.generated.h"

// ========================================================================
// UNesAPU
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================

// Forward Declarations
class ANesMain;

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class NESUNREAL_API UNesApuSoundOutput final : public USynthComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int Count = 0;
	
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	// Sets the oscillator's frequency
	UFUNCTION(BlueprintCallable, Category = "Synth|Components|Audio")
	void SetFrequency(const float FrequencyHz = 440.0f);

	// Length table constant
	static constexpr uint LengthTable[] = {
		10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30}
	;

	UPROPERTY()
	ANesMain* Main;

protected:
	// A simple oscillator class. Can also generate Saw/Square/Tri/Noise.
	Audio::FOsc Osc;
};