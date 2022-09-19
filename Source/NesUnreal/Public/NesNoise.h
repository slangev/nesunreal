// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesSoundChannel.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesNoise final: FNesSoundChannel
{
public:
	NesNoise();
	~NesNoise();
	virtual void Tick() override;
	virtual void HalfFrameTick() override;
	virtual void QuarterFrameTick() override;
	virtual void LengthTick() override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual void Enabled(bool bEnabled) override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
private:
	bool GateCheck();
	bool bLengthCounterHalt;
	bool bConstantVol;
	uint16 Volume; // values of 0-15 with 0 being muted
	uint16 LengthCounter; // APU Length Counter
	uint16 LengthLoad;
	bool bChannelEnabled = false;
};
