// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesSoundChannel.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesTriangle final: FNesSoundChannel
{
public:
	NesTriangle();
	~NesTriangle();
	virtual void Tick() override;
	virtual void HalfFrameTick() override;
	virtual void QuarterFrameTick() override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual void Enabled(bool bEnabled) override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
private:
	bool GateCheck();
	// void EnvelopeTick();
	// void SweepTick();
	// void LengthTick();

	bool bLengthCounterHalt;
	bool bConstantVol;
	uint16 Volume; // values of 0-15 with 0 being muted
	uint16 LengthCounter; // APU Length Counter
	uint16 LengthLoad;
	bool bChannelEnabled = false;


	// Length table constant
	static constexpr uint LengthTable[] = {10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 
	12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
};
