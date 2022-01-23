// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesPulse.h"
#include <memory>
#include "Components/SynthComponent.h"
#include "DSP/Osc.h"
#include "NesAPU.generated.h"

// ========================================================================
// UNesAPU
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class NESUNREAL_API UNesApu : public USynthComponent
{
	GENERATED_BODY()

public:
	void Step(uint Cycle);
	
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	// Sets the oscillator's frequency
	UFUNCTION(BlueprintCallable, Category = "Synth|Components|Audio")
	void SetFrequency(const float FrequencyHz = 440.0f);


protected:
	// A simple oscillator class. Can also generate Saw/Square/Tri/Noise.
	Audio::FOsc Osc;
	int Count = 0;

	std::unique_ptr<FNesPulse> Pulse1;
};