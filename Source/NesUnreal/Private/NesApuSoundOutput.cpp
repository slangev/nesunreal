
#include "NesApuSoundOutput.h"

#include "NesMain.h"

bool UNesApuSoundOutput::Init(int32& SampleRate)
{
	NumChannels = 1;
	// Initialize the DSP objects
	SampleRate = 44100;
	Count = 0;
	Main = Cast<ANesMain>(GetOwner());
	
	UE_LOG(LogTemp,Warning, TEXT("SampleRate: %d"), SampleRate);
	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
	return true;
}

int32 UNesApuSoundOutput::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	if(Count == 0)
	{
		Main->Print();
		Count++;
	}
	
	// Perform DSP operations here
	for (int32 Sample = 0; Sample < NumSamples; ++Sample)
	{
		OutAudio[Sample] = Osc.Generate();
	}
	return NumSamples;
}

void UNesApuSoundOutput::SetFrequency(const float InFrequencyHz)
{
	// Use this protected base class method to push a lambda function which will safely execute in the audio render thread.
	SynthCommand([this, InFrequencyHz]()
	{
		Osc.SetFrequency(InFrequencyHz);
		Osc.Update();
	});
}
