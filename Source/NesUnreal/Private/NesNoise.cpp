// Fill out your copyright notice in the Description page of Project Settings.


#include "NesNoise.h"

NesNoise::NesNoise()
{
    LengthCounter = 0;
}

NesNoise::~NesNoise()
{
}

void NesNoise::Tick() 
{
	if(Timer.Counter > 0)
	{
		Timer.Counter--;
	}
	else
	{
		// Feedback is calculated as the exclusive-OR of bit 0 and one other bit: bit 6 if Mode flag is set, otherwise bit 1.
		uint8 Feedback = (bModeFlag) ? ((ShiftRegister & 0x1 ) ^ (ShiftRegister & 0x40) >> 5) : ((ShiftRegister & 0x1 ) ^ (ShiftRegister & 0x2) >> 1);

		// The shift register is shifted right by one bit.
		ShiftRegister = ShiftRegister >> 1;

		// Bit 14, the leftmost bit, is set to the feedback calculated earlier.
		ShiftRegister = (ShiftRegister | (Feedback << 14));

		Timer.Counter = Timer.Reload;
	}
}

void NesNoise::QuarterFrameTick() 
{
    EnvelopeTick();
}

void NesNoise::HalfFrameTick() 
{
    EnvelopeTick();
    LengthTick();
}

void NesNoise::Enabled(bool bEnabled) 
{
    this->bChannelEnabled = bEnabled;
    if(!bChannelEnabled)
	{
		LengthCounter = 0;
	}
}

void NesNoise::Write(unsigned short Address, uint8 Data) 
{
    switch(Address) 
    {
        case 0x400C:
            Volume = Data & 0xF;
			bConstantVol = (Data & 0x10) == 0x10;
			bLengthCounterHalt = (Data & 0x20) == 0x20;
            break;
        case 0x400D:
            break;
        case 0x400E:
		{
			bModeFlag = (Data & 0x80) == 0x80;
			uint16 NoisePeriod = NoisePeriodTable[Data & 0xF];
			Timer.Reload = NoisePeriod; // Might be wrong.
			Timer.Counter = NoisePeriod;
			break;
		}
        case 0x400F:
			LengthLoad = (Data & 0xF8) >> 3;
            if(bChannelEnabled) 
			{
			    LengthCounter = LengthTable[LengthLoad];
			}
			Envelope.bStart = true;   // restart envelope
            break;
        default:
            break;
    }
}

bool NesNoise::GateCheck() 
{
	if(!LengthAboveZero())
	{
		return true;
	}

	if((ShiftRegister & 0x1) == 0x1) 
	{
		return true;
	}

	return false;
}

bool NesNoise::LengthAboveZero() 
{
    return LengthCounter > 0;
}

int NesNoise::GetOutputVol() 
{
	int CurrVol = 0;
	// Check constant volume here
	if (bConstantVol)
	{
		CurrVol = Volume;
	}
	else 
	{
		CurrVol = Envelope.EnvelopeVol;
	}
	if(GateCheck())
	{
		CurrVol = 0;
	}
    return CurrVol;
}

void NesNoise::LengthTick()
{
    if(!bLengthCounterHalt && LengthCounter > 0)
	{
		LengthCounter--;
	} 
}

void NesNoise::EnvelopeTick()
{
	if(Envelope.bStart)
	{
		Envelope.bStart = false;
		Envelope.DelayLevelCounter = 15;
		Envelope.Timer.Counter = Volume;
	}
	else
	{
		if(Envelope.Timer.Counter > 0 )
		{
			Envelope.Timer.Counter--; // divider is clocked.
		}
		else
		{
			Envelope.Timer.Counter = Volume;
			if(Envelope.DelayLevelCounter > 0)
			{
				Envelope.DelayLevelCounter--;
			}
			else
			{
				if(bLengthCounterHalt)
				{
					Envelope.DelayLevelCounter = 15;
				}
			}
		}
	}
	Envelope.EnvelopeVol = Envelope.DelayLevelCounter;
}