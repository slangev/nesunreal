// Fill out your copyright notice in the Description page of Project Settings.


#include "NesTriangle.h"

NesTriangle::NesTriangle()
{
}

NesTriangle::~NesTriangle()
{
}

void NesTriangle::Tick()
{
    //The sequencer is clocked by the timer as long as both the linear counter and the length counter are nonzero. 
    //The sequencer is clocked by a timer whose period is the 11-bit value (%HHH.LLLLLLLL) formed by timer high and timer low, plus one
    if(Sequencer.Timer.Counter+1 > 0 && LengthAboveZero())
    {
        Sequencer.Timer.Counter--;
    }
    else
    {
        //Sequencer.SequencePointer = (Sequencer.SequencePointer+1) & 0x7;
        Sequencer.Timer.Counter = Sequencer.Timer.Reload;
    }
}

void NesTriangle::QuarterFrameTick() {

}

void NesTriangle::HalfFrameTick() {

}

void NesTriangle::Write(unsigned short Address, uint8 Data) {
    switch(Address) {
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
            bLinearCountReload = true;
            break;
        default:
            break;
    }
}

void NesTriangle::Enabled(bool bEnabled) {
    this->bChannelEnabled = bEnabled;
}

bool NesTriangle::GateCheck() {
    return true;
}

bool NesTriangle::LengthAboveZero() {
    return LengthCounter > 0;
}

int NesTriangle::GetOutputVol() {
    return 0;
}