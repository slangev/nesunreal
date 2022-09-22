// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesPulse.h"
#include "NesTriangle.h"
#include "NesNoise.h"
#include <memory>
#include <vector>

#include "FNesAudioMixer.h"
#include "NesApuFilters.h"
#include "Components/SynthComponent.h"
#include "DSP/Osc.h"
#include "NesApu.generated.h"

// ========================================================================
// UNesAPU
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================
using namespace std;
UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class NESUNREAL_API UNesApu final : public USynthComponent
{
	GENERATED_BODY()

public:
	virtual ~UNesApu() override;
	void QuarterTick();
	void HalfTick();
	void Step(uint CpuCycle);
	void Write(const unsigned short Address, uint8 Data);
	uint8 Read(unsigned short Address);

	UPROPERTY()
	int Count = 0;
	UPROPERTY()
	bool bFiveStepMode = false; // 5-Step Sequence (bit 7 of $4017 set)
	UPROPERTY()
	bool bIRQInhibit = false; // Interrupt inhibit flag. If set, the frame interrupt flag is cleared, otherwise it is unaffected.
	UPROPERTY()
	bool bOddCPUCycle = false;
	UPROPERTY()
	uint32 CPUCycleCount = 0;
	UPROPERTY()
	uint32 ApuCycleCount = 0;
	UPROPERTY()
	int APUBufferCount;
	vector<float> SoundBuffer;
	
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
	std::unique_ptr<FNesPulse> Pulse1;
	std::unique_ptr<FNesPulse> Pulse2;
	std::unique_ptr<NesTriangle> Triangle;
	std::unique_ptr<NesNoise> Noise;
	std::unique_ptr<FNesAudioMixer> Mixer;
	std::unique_ptr<FNesApuFilters> Filter;
	bool WriteSamples = false;
	const float AudioSampleRate = 44100.0f;
	const float CPUSpeed = 1790000.0f; //1.79MHz
	const int Speed = (CPUSpeed / AudioSampleRate);
};