#include "NesApu.h"

#include "NesMain.h"
#include "NesCPU.h"
#include "GenericPlatform/GenericPlatformProcess.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesApu,Log,All)

bool UNesApu::Init(int32& SampleRate)
{
	NumChannels = 1;
	// Initialize the DSP objects
	SampleRate = AudioSampleRate;
	
	PreferredBufferLength = 2048;
	bAllowSpatialization = false;
	
	Count = 0;
	Pulse1 = std::make_unique<FNesPulse>();
	Pulse1->SetChannelId(1);
	Pulse2 = std::make_unique<FNesPulse>();
	Pulse2->SetChannelId(2);
	Triangle = std::make_unique<NesTriangle>();
	Mixer = std::make_unique<FNesAudioMixer>();
	Filter = std::make_unique<FNesApuFilters>();
	for(int i = 0; i < PreferredBufferLength; i++)
	{
		SoundBuffer.push_back(0);
	}
	
	UE_LOG(LogNesApu, Display, TEXT("SampleRate: %d"), SampleRate);
	UE_LOG(LogNesApu, Display, TEXT("SoundBuffer size: %d"), SoundBuffer.size());
	UE_LOG(LogNesApu, Display, TEXT("Speed: %d"), Speed);

	Osc.Init(SampleRate);
	Osc.SetFrequency(440.0f);
	Osc.Start();
	return true;
}

int32 UNesApu::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	if(WriteSamples) {
		for(int i = 0; i < NumSamples; i++)
		{
			OutAudio[i] = SoundBuffer.at(i);
		}
		WriteSamples = false;
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
	Triangle->QuarterFrameTick();
}

void UNesApu::HalfTick()
{
	Pulse1->HalfFrameTick();
	Pulse2->HalfFrameTick();
	Triangle->HalfFrameTick();
}

void UNesApu::Step(uint32 CpuCycle)
{
	while (CpuCycle-- > 0)
	{
		bOddCPUCycle = !bOddCPUCycle;
		CPUCycleCount++;

		if (bOddCPUCycle)
		{
			ApuCycleCount++;
			Pulse1->Tick();
			Pulse2->Tick();
		}
		// Triangle is tick every CPU cycle.
		Triangle->Tick();

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

		if (APUBufferCount/Speed >= SoundBuffer.size()){
			APUBufferCount = 0;
			WriteSamples = true;
		}
		while(WriteSamples) {
			 FPlatformProcess::Sleep(0);
		}
		if (APUBufferCount % Speed == 0) {
			float SampleOutput = 0;
			const float SquareOutputVal = Mixer->LookupPulseTable(Pulse1->GetOutputVol(), Pulse2->GetOutputVol());
			const float TriangleOutputVal = Mixer->LookupTndTable(Triangle->GetOutputVol(),0,0);
			Filter->HighPassFilter(AudioSampleRate,90.0f);
			float Sample = Filter->Step(SquareOutputVal + TriangleOutputVal);
			Filter->HighPassFilter(AudioSampleRate,440.0f);
			Sample = Filter->Step(Sample);
			Filter->LowPassFilter(AudioSampleRate,14000.0f);
			Sample = Filter->Step(Sample);
			Sample = FMath :: Clamp ( Sample, -1.0f, 1.0f);
			SoundBuffer.at(APUBufferCount / Speed) = Sample;
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
		Triangle->Write(Address, Data);
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
		Triangle->Enabled((Data & 0x4) >> 2);
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
	uint8 status = 0;
	if(Address == 0x4015)
	{
		bool bPulse1Enabled = Pulse1->LengthAboveZero();
		bool bPulse2Enabled = Pulse2->LengthAboveZero();
		bool bTriangle1Enabled = Triangle->LengthAboveZero();
		status = (bPulse1Enabled) ? FNesCPU::SetBit(0, status) : FNesCPU::ResetBit(0, status);
		status = (bPulse2Enabled) ? FNesCPU::SetBit(1, status) : FNesCPU::ResetBit(1, status);
		status = (bTriangle1Enabled) ? FNesCPU::SetBit(2, status) : FNesCPU::ResetBit(2, status);
	}
	return status;
}
