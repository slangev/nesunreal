// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 * https://wiki.nesdev.org/w/index.php?title=APU
 * https://wiki.nesdev.org/w/index.php?title=APU_Pulse
 * https://wiki.nesdev.org/w/index.php?title=APU_Length_Counter
 * https://wiki.nesdev.org/w/index.php?title=APU_Envelope
 * https://wiki.nesdev.org/w/index.php?title=APU_Triangle
 * https://wiki.nesdev.org/w/index.php?title=APU_Frame_Counter
 * https://wiki.nesdev.org/w/index.php?title=APU_Mixer
 */
class NESUNREAL_API FNesSoundChannel
{
public:
	FNesSoundChannel();
	virtual ~FNesSoundChannel();
	virtual void Tick()=0;
	virtual void HalfFrameTick()=0;
	virtual void QuarterFrameTick()=0;
	virtual void Write(unsigned short Address, uint8 Data)=0;
	virtual void Enabled(bool bEnabled)=0;
	virtual int GetOutputVol()=0;
	virtual bool LengthAboveZero()=0;
};
