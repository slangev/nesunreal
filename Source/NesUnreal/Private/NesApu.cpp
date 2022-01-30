
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
	Pulse1->SetChannelId(1);
	Pulse2 = std::make_unique<FNesPulse>();
	Pulse2->SetChannelId(2);
	Mixer = std::make_unique<FNesAudioMixer>();
	for(int i = 0; i < 1024; i++)
	{
		SoundBuffer.push_back(0);
	}
	
	UE_LOG(LogTemp,Warning, TEXT("SampleRate: %d"), SampleRate);
	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
	return true;
}

int32 UNesApu::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	
	// Perform DSP operations here
	if(WriteBuffer)
	{
		for(int i = 0; i < SoundBuffer.size(); i++)
		{
			OutAudio[i] = SoundBuffer[i];
		}
		WriteBuffer = false;
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

void UNesApu::Step(uint32 Cycle)
{
	while (Cycle-- > 0)
	{
		bOddCPUCycle = !bOddCPUCycle;
		CPUCycleCount++;

		if (bOddCPUCycle){
			ApuCycleCount++;
			Pulse1->Tick();
			Pulse2->Tick();
		}

		// TODO: Need to tick Triangle

		if(bFiveStepMode)
		{
			switch(ApuCycleCount)
			{
			case 3729:
				{
					Pulse1->QuarterFrameTick();
					Pulse2->QuarterFrameTick();
					break;
				}
			case 7457:
				{
					Pulse1->QuarterFrameTick();
					Pulse2->QuarterFrameTick();
					Pulse1->HalfFrameTick();
					Pulse2->HalfFrameTick();
					break;
				}
			case 11186:
				{
					Pulse1->QuarterFrameTick();
					Pulse2->QuarterFrameTick();
					break;
				}
			case 18641:
				{
					Pulse1->QuarterFrameTick();
					Pulse2->QuarterFrameTick();
					Pulse1->HalfFrameTick();
					Pulse2->HalfFrameTick();
					ApuCycleCount = 0;
					break;
				}
			}
		} else
		{
			switch(ApuCycleCount)
			{
				case 3729:
					{
						Pulse1->QuarterFrameTick();
						Pulse2->QuarterFrameTick();
						break;
					}
				case 7457:
					{
						Pulse1->QuarterFrameTick();
						Pulse2->QuarterFrameTick();
						Pulse1->HalfFrameTick();
						Pulse2->HalfFrameTick();
						break;
					}
				case 11186:
					{
						Pulse1->QuarterFrameTick();
						Pulse2->QuarterFrameTick();
						break;
					}
				case 14915:
					{
						Pulse1->QuarterFrameTick();
						Pulse2->QuarterFrameTick();
						Pulse1->HalfFrameTick();
						Pulse2->HalfFrameTick();
						ApuCycleCount = 0;
						break;
					}
			}
		}

		// Add Sample to queue
		constexpr int Speed = 40;
		if (APUBufferCount/Speed >= SoundBuffer.size()){
			APUBufferCount = 0;
			WriteBuffer = true;
		}
		if (true) {
			const float SampleOutput = Mixer->LookupPulseTable(Pulse1->GetOutputVol(), Pulse2->GetOutputVol());
			SoundBuffer.at(APUBufferCount / Speed) = SampleOutput;
		}
		APUBufferCount++;
	}
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
		Pulse1->Enabled(Data & 0x1);
		Pulse2->Enabled((Data & 0x2) >> 1);
	}
	else if(Address == 0x4017)
	{
		bFiveStepMode = (Data & 0x80) == 0x80 ? true : false;
		bIRQInhibit = (Data & 0x40) == 0x40;
		ApuCycleCount = 0;
		if (bFiveStepMode){
			Pulse1->QuarterFrameTick();
			Pulse2->QuarterFrameTick();
			Pulse1->HalfFrameTick();
			Pulse2->HalfFrameTick();
		}
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
