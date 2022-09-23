// Fill out your copyright notice in the Description page of Project Settings.


#include "NesDMC.h"

NesDMC::NesDMC()
{
    LengthCounter = 0;
}

NesDMC::~NesDMC()
{
}

void NesDMC::Tick() 
{
    if(Timer.Counter > 0)
	{
		Timer.Counter--;
	}
}

void NesDMC::HalfFrameTick()
{

}

void NesDMC::QuarterFrameTick()
{

}

void NesDMC::Write(unsigned short Address, uint8 Data) 
{

}

void NesDMC::Enabled(bool bEnabled)
{
    this->bChannelEnabled = bEnabled;
    if(!bChannelEnabled)
	{
		LengthCounter = 0;
	}

}

void NesDMC::LengthTick()
{

}

bool NesDMC::LengthAboveZero()
{
    return LengthCounter > 0;
}

int NesDMC::GetOutputVol() 
{
    return 0;
}