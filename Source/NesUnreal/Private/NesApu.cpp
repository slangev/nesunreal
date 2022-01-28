
#include "NesApu.h"

#include "NesMain.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesAPU,Log,All)

bool UNesApu::Init(int32& SampleRate)
{
	NumChannels = 1;
	// Initialize the DSP objects
	SampleRate = 44100;
	Count = 0;
	Pulse1 = std::make_unique<FNesPulse>();
	Pulse2 = std::make_unique<FNesPulse>();
	Mixer = std::make_unique<FNesAudioMixer>();
	
	UE_LOG(LogTemp,Warning, TEXT("SampleRate: %d"), SampleRate);
	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
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

UNesApu::~UNesApu()
{
	
}

void UNesApu::Step(uint Cycle)
{
	Count = 10;
	UE_LOG(LogTemp,Warning, TEXT("HERE %d"), Count);
}

void UNesApu::Write(const unsigned short Address, uint8 Data)
{
	if(Address >= 0x4000 && Address <= 0x4003)
	{
		Pulse1->Write(Address, Data);
	}
	else if(Address >= 0x4004 && Address <= 0x4007)
	{
		Pulse2->Write(Address, Data);
	}
	else if(Address >= 0x4008 && Address <= 0x400B)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Triangle. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x400C && Address <= 0x400F)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Noise. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x4010 && Address <= 0x4013)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to DMC. Address: %d Data: %d"), Address, Data);
	}
	else if(Address == 0x4015)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Status. Address: %d Data: %d"), Address, Data);
		bFiveStepMode = (Data & 0x80) == 0x80 ? true : false;
		bIRQInhibit = (Data & 0x40) == 0x40;
	}
	else if(Address == 0x4017)
	{	
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Frame Counter. Address: %d Data: %d"), Address, Data);
	}
	else
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Bad Write. Address: %d Data: %d"), Address, Data);
	}
}

uint8 UNesApu::Read(const unsigned short Address)
{
	if(Address == 0x4015)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Reading to Status. Address: %d"), Address);
	}
	return 0;
}
