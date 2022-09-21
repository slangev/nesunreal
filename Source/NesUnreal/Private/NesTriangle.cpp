// Fill out your copyright notice in the Description page of Project Settings.


#include "NesTriangle.h"

NesTriangle::NesTriangle()
{
    LengthCounter = 0;
}

NesTriangle::~NesTriangle()
{
}

void NesTriangle::Tick()
{
    //The sequencer is clocked by the timer as long as both the linear counter and the length counter are nonzero. 
    //The sequencer is clocked by a timer whose period is the 11-bit value (%HHH.LLLLLLLL) formed by timer high and timer low, plus one
    if(GateCheck())
    {
        if(Sequencer.Timer.Counter+1 > 0) 
        {
            Sequencer.Timer.Counter--;
        }
        else
        {
            Sequencer.SequencePointer = (Sequencer.SequencePointer+1) & 0x1F;
            Sequencer.Timer.Counter = Sequencer.Timer.Reload;
        }
    }
}

void NesTriangle::QuarterFrameTick() 
{
    LinearTick();
}

void NesTriangle::HalfFrameTick() 
{
    LinearTick();
    LengthTick();
}

void NesTriangle::Write(unsigned short Address, uint8 Data) 
{
    switch(Address) 
    {
        case 0x4008:
            CounterReloadValue = Data & 0x7F;
            bLengthCounterHalt = (Data & 0x80) == 0x80;
            break;
        case 0x4009:
            break;
        case 0x400A:
        	Sequencer.TimerLow = Data & 0xFF;
			Sequencer.Timer.Reload = (Sequencer.Timer.Reload & 0x700) | Sequencer.TimerLow;
            break;
        case 0x400B:
            Sequencer.TimerHigh = Data & 0x07;
			Sequencer.Timer.Reload = (Sequencer.TimerHigh << 8) | Sequencer.TimerLow;
            if(bChannelEnabled) 
			{
			    LengthCounter = LengthTable[LengthLoad];
			}
            bLinearCountReload = true;
            break;
        default:
            break;
    }
}

void NesTriangle::Enabled(bool bEnabled) 
{
    this->bChannelEnabled = bEnabled;
    if(!bChannelEnabled)
	{
		LengthCounter = 0;
	}
}

bool NesTriangle::GateCheck() 
{
    if(LengthAboveZero() && LinearAboveZero()) 
    {
        return true;
    }
    return false;
}

bool NesTriangle::LengthAboveZero() 
{
    return LengthCounter > 0;
}

int NesTriangle::GetOutputVol() 
{
    int CurrVol = StepSequence[Sequencer.SequencePointer];
	return CurrVol;
}

void NesTriangle::LengthTick()
{
    if(!bLengthCounterHalt && LengthCounter > 0)
	{
		LengthCounter--;
	} 
}

void NesTriangle::LinearTick() 
{
    //If the linear counter reload flag is set, the linear counter is reloaded with the counter reload value, otherwise if the linear counter is non-zero, it is decremented.
    if(bLinearCountReload) 
    {
        LinearCounter = CounterReloadValue;
    } 
    else if(LinearCounter > 0) 
    {
        LinearCounter--;
    }

    //If the control flag is clear (Control flag (this bit is also the length counter halt flag)) , the linear counter reload flag is cleared.
    if(!bLengthCounterHalt) 
    {
        bLinearCountReload = false;
    }
}