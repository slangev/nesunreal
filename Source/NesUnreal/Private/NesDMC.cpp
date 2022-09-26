// Fill out your copyright notice in the Description page of Project Settings.


#include "NesDMC.h"

NesDMC::NesDMC()
{
    LengthCounter = 0;
}

NesDMC::~NesDMC()
{
}

void NesDMC::AttachCart(shared_ptr<NesCart> Cart) 
{
    this->M_Cart = Cart;
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
    // Not used
}

void NesDMC::QuarterFrameTick()
{
    // Not used
}

void NesDMC::Write(unsigned short Address, uint8 Data) 
{
    switch(Address) 
    {
        case 0x4010:
            bIRQEnabled = (Data & 0x80) == 0x80;
            bLoopFlag = (Data & 0x40) == 0x40;
            RateIndex = (Data & 0xF);
            break;
        case 0x4011:
            Output = (Data & 0x7F);
            break;
        case 0x4012:
            SampleAddress = 0xC000 + (Data * 64);
			break;
        case 0x4013:
            SampleLength = (Data * 16) + 1; // bytes
            break;
        default:
            break;
    }
}

void NesDMC::Enabled(bool bEnabled)
{
    this->bChannelEnabled = bEnabled;
    if (!this->bChannelEnabled){
        SampleLength = 0;
    }
    // else{
    //     if (sampleLength == 0){
    //         sampleLength = sampleLengthLoad;
    //         sampleAddress = sampleAddressLoad;
    //     }
    // }
    // IRQ = false;
}

void NesDMC::LengthTick()
{

}

bool NesDMC::LengthAboveZero()
{
    return SampleBuffer > 0;
}

int NesDMC::GetOutputVol() 
{
    return Output;
}