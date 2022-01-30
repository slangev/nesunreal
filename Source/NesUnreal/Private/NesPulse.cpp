// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPulse.h"

FNesPulse::FNesPulse(): ChannelId(1), bLengthCounterHalt(false), bConstantVol(false), Volume(0), LengthCounter(0),
                        LengthLoad(0),
                        Sweep(),
                        Envelope(), Sequencer()
{
}

FNesPulse::~FNesPulse()
{
	
}

// For Pulse, Tick are for the sequencer.
void FNesPulse::Tick()
{
	if(Sequencer.Timer.Counter > 0)
	{
		Sequencer.Timer.Counter--;
	}
	else
	{
		Sequencer.SequencePointer = (Sequencer.SequencePointer+1) & 0x7;
		Sequencer.Timer.Counter = Sequencer.Timer.Reload;
	}
}

void FNesPulse::HalfFrameTick()
{
	SweepTick();
	LengthTick();
}

void FNesPulse::QuarterFrameTick()
{
	EnvelopeTick();
}

void FNesPulse::Write(unsigned short Address, uint8 Data)
{
	switch(Address)
	{
	case 0x4000:
	case 0x4004:
		{
			Volume = Data & 0xF;
			bConstantVol = (Data & 0x10) == 0x10;
			bLengthCounterHalt = (Data & 0x20) == 0x20;
			Sequencer.Duty = (Data >> 6) & 0x3;
			break;
		}
	case 0x4001:
	case 0x4005:
		{
			Sweep.SweepShiftCount = Data & 0x7;
			Sweep.bSweepEnable = (Data & 0x80) == 0x80 && Sweep.SweepShiftCount != 0;
			Sweep.SweepDividerPeriod = ((Data >> 4) & 0x7);
			Sweep.bSweepNegate = (Data & 0x8) == 0x8;
			Sweep.bSweepReload = true;
			break;
		}
	case 0x4002:
	case 0x4006:
		{
			Sequencer.TimerLow = Data & 0xFF;
			Sequencer.Timer.Reload = (Sequencer.Timer.Reload & 0x700) | Sequencer.TimerLow;
			break;
		}
	case 0x4003:
	case 0x4007:
		{
			Sequencer.TimerHigh = Data & 0x07;
			Sequencer.Timer.Reload = (Sequencer.TimerHigh << 8) | Sequencer.TimerLow;
			LengthLoad = (Data & 0xF8) >> 3;
			if (bChannelEnabled) {
				LengthCounter = LengthTable[LengthLoad];
			}
			/* Side Effects */
			Sequencer.SequencePointer = 0;
			Sequencer.Timer.Counter = Sequencer.Timer.Reload;
			Envelope.bStart = true;   // sets start on envelope
			break;
		}
	default:
		{
			UE_LOG(LogTemp,Warning,TEXT("PULSE 1 bad address write"));
		}
	}
}

void FNesPulse::Enabled(const bool bEnabled)
{
	this->bChannelEnabled = bEnabled;
	if(!bChannelEnabled)
	{
		LengthCounter = 0;
	}
}

int FNesPulse::GetOutputVol()
{
	int CurrVol;
	// Check constant volume here
	if (bConstantVol){
		CurrVol = Volume;
	}
	else {
		CurrVol = Envelope.EnvelopeVol;
	}
	if(GateCheck())
	{
		CurrVol = 0;
	}
	return CurrVol;
}

bool FNesPulse::LengthAboveZero()
{
	return LengthCounter > 0;
}

bool FNesPulse::IsMuted(const uint TargetPeriod) const
{
	if(Sequencer.Timer.Reload < 8 || TargetPeriod > 0x7FF)
	{
		return true;
	}
	return false;
}

/*
*The mixer receives the current envelope volume except when

The sequencer output is zero, or
overflow from the sweep unit's adder is silencing the channel, or
the length counter is zero, or
the timer has a value less than eight.
*/
bool FNesPulse::GateCheck()
{
	//Sweep gate/Seq/LengthCounter
	if(Sweep.bSweepMuting)
	{
		return true;
	}
	
	if(OutputWaveformTable[Sequencer.Duty][Sequencer.SequencePointer] == 0)
	{
		return true;
	}

	if(!LengthAboveZero())
	{
		return true;
	}
	
	return false;
}

void FNesPulse::EnvelopeTick()
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

void FNesPulse::SweepTick()
{
	uint ChangeAmount = Sequencer.Timer.Reload >> Sweep.SweepShiftCount;
	uint TargetPeriod = Sequencer.Timer.Reload;
	if(Sweep.bSweepNegate)
	{
		ChangeAmount = -ChangeAmount;
		if(ChannelId == 1)
		{
			ChangeAmount = ChangeAmount - 1;
		}
	}
	TargetPeriod += ChangeAmount;
	Sweep.bSweepMuting = IsMuted(TargetPeriod);
	if(Sweep.SweepDividerCounter == 0 && Sweep.bSweepEnable && !Sweep.bSweepMuting)
	{
		Sequencer.Timer.Reload = TargetPeriod;
		Sequencer.TimerHigh = (Sequencer.Timer.Reload & 0x700) >> 8;
		Sequencer.TimerLow = Sequencer.Timer.Reload & 0xFF;
	}
	if(Sweep.SweepDividerCounter == 0 || Sweep.bSweepReload)
	{
		Sweep.SweepDividerCounter = Sweep.SweepDividerPeriod;
		Sweep.bSweepReload = false;
	}
	else
	{
		Sweep.SweepDividerCounter--;
	}
}

void FNesPulse::LengthTick()
{
	if(!bLengthCounterHalt && LengthCounter > 0)
	{
		LengthCounter--;
	} 
}
