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
	void LinearTick();
	bool LinearAboveZero() {return LinearCounter > 0;};

	bool bLinearCountReload = false;
	uint16 CounterReloadValue = 0; 
	uint16 LinearCounter = 0;

	struct FDivider
	{
		uint16 Reload; // timer load 
		int16 Counter; // Countdown timer
	};

	struct FSequencer
	{
		uint8 SequencePointer = 0; // index for StepSequence table
		FDivider Timer;
		uint16 TimerLow = 0;
		uint16 TimerHigh = 0;
	};

	FSequencer Sequencer;

	// StepSequence
	static constexpr uint StepSequence[] = 
	{
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 0, 
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
	};
};
