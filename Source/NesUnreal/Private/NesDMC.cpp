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
    else 
    {
        // Reload timer
        Timer.Counter = Timer.Reload;

        /*
                        DMA Reader

            When the sample buffer is in an empty state and the bytes counter is non-zero,
            the following occur: The sample buffer is filled with the next sample byte read
            from memory at the current address, subject to whatever mapping hardware is
            present (the same as CPU memory accesses). The address is incremented; if it
            exceeds $FFFF, it is wrapped around to $8000. The bytes counter is decremented;
            if it becomes zero and the loop flag is set, the sample is restarted (see
            above), otherwise if the bytes counter becomes zero and the interrupt enabled
            flag is set, the interrupt flag is set.

            When the DMA reader accesses a byte of memory, the CPU is suspended for 4 clock
            cycles.
        */
       
       if(SampleBuffer == 0 && CurrentLength > 0) 
       {
            // When the DMA reader accesses a byte of memory, the CPU is suspended for 4 clock cycles. 
            SampleBuffer = M_Cart->Read(CurrentAddress);
            bAccessedMemory = true;
            CurrentAddress++;
            if(CurrentAddress == 0) 
            {
                CurrentAddress = 0x8000;
            }

            // The bytes counter is decremented; if it becomes zero and the loop flag is set, the sample is restarted
            CurrentLength--;
            // When the DMC sample is restarted, the address counter is set to register $4012 * $40 + $C000 and the bytes counter is set to register $4013 * $10 + 1.
            if(CurrentLength == 0 && bLoopFlag)
            {
                CurrentAddress = SampleAddress;
                CurrentLength = SampleLength;
            }
            // otherwise if the bytes counter becomes zero and the interrupt enabled flag is set, the interrupt flag is set.
            else if(CurrentLength == 0 && bIRQEnabled) 
            {
                // TODO: SET INTERRUPT FLAG
            }
       }
        /*
                        Output Unit
            
            When an output cycle is started, the counter is loaded with 8 and if the sample
            buffer is empty, the silence flag is set, otherwise the silence flag is cleared
            and the sample buffer is emptied into the shift register.

            On the arrival of a clock from the timer, the following actions occur in order:

                1. If the silence flag is clear, bit 0 of the shift register is applied to
            the DAC counter: If bit 0 is clear and the counter is greater than 1, the
            counter is decremented by 2, otherwise if bit 0 is set and the counter is less
            than 126, the counter is incremented by 2.

                1) The shift register is clocked.
                
                2) The counter is decremented. If it becomes zero, a new cycle is started.
        */
       if(!bSilence) 
       {
            uint8 bit0 = ShiftRegister & 0x1;
            if(bit0 == 0 && Output > 1) 
            {
                Output = Output - 2;
            } 
            else if(bit0 == 1 && Output < 126)
            {
                Output = Output + 2;
            } 
            // The shift register is clocked.
            ShiftRegister = ShiftRegister >> 1;
            // The counter is decremented. If it becomes zero, a new cycle is started.
            BitRemaining--;
       }

       /*
            When an output cycle ends, a new cycle is started as follows:

            The bits-remaining counter is loaded with 8.
            If the sample buffer is empty, then the silence flag is set; otherwise, the silence flag is cleared and the sample buffer is emptied into the shift register.

            When the timer outputs a clock, the following actions occur in order:

            If the silence flag is clear, the output level changes based on bit 0 of the shift register. If the bit is 1, add 2; otherwise, subtract 2. But if adding or subtracting 2 would cause the output level to leave the 0-127 range, leave the output level unchanged. This means subtract 2 only if the current level is at least 2, or add 2 only if the current level is at most 125.
            The right shift register is clocked.
            As stated above, the bits-remaining counter is decremented. If it becomes zero, a new output cycle is started.

        */
       if(BitRemaining == 0)
       {
            BitRemaining = 8;
            if(SampleBuffer == 0) 
            {
                bSilence = true;
            } 
            else
            {
                bSilence = false;
                ShiftRegister = SampleBuffer;
                SampleBuffer = 0;
            }
       }    
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
            Timer.Reload = RateIndexTable[RateIndex]; // Might be wrong.
			Timer.Counter = RateIndexTable[RateIndex];
            break;
        case 0x4011:
            Output = (Data & 0x7F);
            break;
        case 0x4012:
            SampleAddress = 0xC000 + (Data * 64);
            CurrentAddress = SampleAddress;
			break;
        case 0x4013:
            SampleLength = (Data * 16) + 1; // bytes
            CurrentLength = SampleLength;
            break;
        default:
            break;
    }
}

void NesDMC::Enabled(bool bEnabled)
{
    /*
        - If the DMC bit is clear, the DMC bytes remaining will be set to 0 and the DMC will silence when it empties.
        - If the DMC bit is set, the DMC sample will be restarted only if its bytes remaining is 0. If there are bits remaining in the 1-byte sample buffer, these will finish playing before the next sample is fetched.
        - Writing to this register clears the DMC interrupt flag.
    */
    this->bChannelEnabled = bEnabled;
    if (!this->bChannelEnabled)
    {
        CurrentLength = 0;
    }
    else
    {
        if(CurrentLength == 0) 
        {
            CurrentAddress = SampleAddress;
            CurrentLength = SampleLength;
        }
    }

}

void NesDMC::LengthTick()
{

}

bool NesDMC::LengthAboveZero()
{
    return CurrentLength > 0;
}

int NesDMC::GetOutputVol() 
{
    return Output;
}