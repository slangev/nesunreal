// Fill out your copyright notice in the Description page of Project Settings.


#include "NesAPU.h"

bool UNesApu::Init(int32& SampleRate)
{
	NumChannels = 1;
	// Initialize the DSP objects
	SampleRate = 44100;

	Pulse1 = std::make_unique<FNesPulse>();
	
	UE_LOG(LogTemp,Warning, TEXT("SampleRate: %d"), SampleRate);
	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
	UE_LOG(LogTemp,Warning, TEXT("OscType: %d"), Osc.GetType());
	Pulse1->Tick();
	return true;
}

int32 UNesApu::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	// Perform DSP operations here
	for (int32 Sample = 0; Sample < NumSamples; ++Sample)
	{
		OutAudio[Sample] = Osc.Generate();
	}
	return NumSamples;
}

void UNesApu::SetFrequency(const float InFrequencyHz)
{
	// Use this protected base class method to push a lambda function which will safely execute in the audio render thread.
	SynthCommand([this, InFrequencyHz]()
	{
		Osc.SetFrequency(InFrequencyHz);
		Osc.Update();
	});
}

void UNesApu::Step(uint Cycle)
{
	
}
