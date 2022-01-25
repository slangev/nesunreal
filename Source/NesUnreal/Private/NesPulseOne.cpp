// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPulseOne.h"

FNesPulseOne::FNesPulseOne(): bLengthCounterHalt(false), bConstantVol(false), Volume(0), LengthCounter(0), Sweep(),
                              Envelope(), Sequencer()
{
}

FNesPulseOne::~FNesPulseOne()
{
	
}

// For Pulse, Tick are for the sequencer.
void FNesPulseOne::Tick()
{
	
}

void FNesPulseOne::HalfFrameTick()
{
}

void FNesPulseOne::QuarterFrameTick()
{
}

void FNesPulseOne::WriteData(const int Address, int Data)
{
	switch(Address)
	{
	case 0x4000:
		{
			Volume = Data & 0xF;
			bConstantVol = (Data & 0x10) == 0x10;
			bLengthCounterHalt = (Data & 0x20) == 0x20;
			Sequencer.Duty = (Data >> 6) & 0x3;
			break;
		}
	case 0x4001:
		{
			Sweep.SweepShiftCount = Data & 0x7;
			Sweep.bSweepNegate = (Data & 0x8) == 0x8;
			Sweep.SweepDividerPeriod = ((Data >> 4) & 0x7);
			Sweep.bSweepEnable = (Data & 0x80) == 0x80 && Sweep.SweepShiftCount != 0;
			Sweep.bSweepReload = true;
			break;
		}
	case 0x4002:
		{
			Sequencer.TimerLow = Data & 0xFF;
			Sequencer.Timer.Reload = (Sequencer.Timer.Reload & 0x700) | Sequencer.TimerLow;
			break;
		}
	case 0x4003:
		{
			Sequencer.TimerHigh = Data & 0x07;
			Sequencer.Timer.Reload = (Sequencer.TimerHigh << 8) | Sequencer.TimerLow;
			LengthCounter = (Data & 0xF8) >> 3;
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

void FNesPulseOne::Enabled(const bool bEnabled)
{
	this->bChannelEnabled = bEnabled;
}

int FNesPulseOne::GetOutputVol()
{
	return 0;
}

bool FNesPulseOne::LengthAboveZero()
{
	return false;
}

bool FNesPulseOne::GateCheck()
{
	return false;
}
