
#include "NesApu.h"

#include "NesMain.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesApu,Log,All)

bool UNesApu::Init(int32& SampleRate)
{
	NumChannels = 1;
	// Initialize the DSP objects
	SampleRate = 44100;
	
	PreferredBufferLength = 1024;
	
	Count = 0;
	Pulse1 = std::make_unique<FNesPulse>();
	Pulse1->SetChannelId(1);
	Pulse2 = std::make_unique<FNesPulse>();
	Pulse2->SetChannelId(2);
	Mixer = std::make_unique<FNesAudioMixer>();
	Filter = std::make_unique<FNesApuFilters>();
	for(int i = 0; i < PreferredBufferLength; i++)
	{
		SoundBuffer.push_back(0);
	}
	
	UE_LOG(LogTemp,Warning, TEXT("SampleRate: %d"), SampleRate);
	UE_LOG(LogTemp,Warning, TEXT("SoundBuffer size: %d"), SoundBuffer.size());

	Osc.Init(SampleRate);
	Osc.Start();
	return true;
}

int32 UNesApu::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{

	// Perform DSP operations here
	for(int i = 0; i < NumSamples; i++)
	{
		OutAudio[i] = SoundBuffer.at(i);
	}
	
	return NumSamples;
}

void UNesApu::SetFrequency(const float FrequencyHz)
{
	// Use this protected base class method to push a lambda function which will safely execute in the audio render thread.
	SynthCommand([this, FrequencyHz]()
	{
		Osc.SetFrequency(FrequencyHz);
		Osc.Update();
	});
}

UNesApu::~UNesApu()
{
	
}

void UNesApu::QuarterTick()
{
	Pulse1->QuarterFrameTick();
	Pulse2->QuarterFrameTick();
}

void UNesApu::HalfTick()
{
	Pulse1->HalfFrameTick();
	Pulse2->HalfFrameTick();
}

void UNesApu::Step(uint32 CpuCycle)
{
	while (CpuCycle-- > 0)
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
					QuarterTick();
					break;
				}
			case 7457:
				{
					QuarterTick();
					HalfTick();
					break;
				}
			case 11186:
				{
					QuarterTick();
					break;
				}
			case 18641:
				{
					QuarterTick();
					HalfTick();
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
						QuarterTick();
						break;
					}
				case 7457:
					{
						QuarterTick();
						HalfTick();
						break;
					}
				case 11186:
					{
						QuarterTick();
						break;
					}
				case 14915:
					{
						QuarterTick();
						HalfTick();
						ApuCycleCount = 0;
						break;
					}
			}
		}

		constexpr int Speed = 40;
		if (APUBufferCount/Speed >= SoundBuffer.size()){
			APUBufferCount = 0;
		}
		if (APUBufferCount % Speed == 0) {
			float SampleOutput = 0;
			if(bOddCPUCycle)
			{
				const float SquareOutputVal = Mixer->LookupPulseTable(Pulse1->GetOutputVol(), Pulse2->GetOutputVol());
				Filter->HighPassFilter(44100.0f,90.0f);
				float Sample = Filter->Step(SquareOutputVal);
				Filter->HighPassFilter(44100.0f,440.0f);
				Sample = Filter->Step(Sample);
				Filter->LowPassFilter(44100.0f,14000.0f);
				Sample = Filter->Step(Sample);
				Sample = FMath :: Clamp ( Sample, -1.0f, 1.0f) / 2.0f;
				SampleOutput = Sample;
			}
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
		//UE_LOG(LogNesAPU,Warning,TEXT("Writing to Triangle. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x400C && Address <= 0x400F)
	{
		//UE_LOG(LogNesAPU,Warning,TEXT("Writing to Noise. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x4010 && Address <= 0x4013)
	{
		//UE_LOG(LogNesAPU,Warning,TEXT("Writing to DMC. Address: %d Data: %d"), Address, Data);
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
			QuarterTick();
			HalfTick();
		}
		//UE_LOG(LogNesAPU,Warning,TEXT("Writing to Frame Counter. Address: %d Data: %d"), Address, Data);
	}
	else
	{
		//UE_LOG(LogNesAPU,Warning,TEXT("Bad Write. Address: %d Data: %d"), Address, Data);
	}
}

uint8 UNesApu::Read(const unsigned short Address)
{
	if(Address == 0x4015)
	{
		UE_LOG(LogNesApu,Warning,TEXT("Reading to Status. Address: %d"), Address);
	}
	return 0;
}
