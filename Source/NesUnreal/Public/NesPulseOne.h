// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesSoundChannel.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesPulseOne final: FNesSoundChannel
{
public:
	FNesPulseOne();
	virtual ~FNesPulseOne() override;
	virtual void Tick() override;
	virtual void HalfFrameTick() override;
	virtual void QuarterFrameTick() override;
	virtual void WriteData(int Address, int Data) override;
	virtual void Enabled(bool bEnabled) override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
private:
	uint8 Duty = 0;
	bool bLengthCounterHalt;
	bool bConstantVol;
	unsigned short Volume;

	// Sweep information
	bool bSweepEnable;
};
