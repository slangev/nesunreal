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
	virtual void LengthTick() override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
private:
	bool GateCheck();

	bool bLengthCounterHalt;
	uint16 LengthCounter; // APU Length Counter
	uint16 LengthLoad;
	bool bChannelEnabled = false;
	bool bLinearCountReload = false;
	uint8 CounterReloadValue = 0; 

	struct FDivider
	{
		uint16 Reload; // timer load 
		uint16 Counter; // Countdown timer
	};

	struct FSequencer
	{
		FDivider Timer;
		uint16 TimerLow = 0;
		uint16 TimerHigh = 0;
	};

	FSequencer Sequencer;

	// Length table constant
	static constexpr uint StepSequence[] = {
	15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 0, 
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
};
