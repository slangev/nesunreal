// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesSoundChannel.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesPulse final: FNesSoundChannel
{
public:
	FNesPulse();
	virtual ~FNesPulse() override;
	virtual void Tick() override;
	virtual void HalfFrameTick() override;
	virtual void QuarterFrameTick() override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual void Enabled(bool bEnabled) override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
	void SetChannelId(const int Id) {this->ChannelId = Id;}
	int GetChannel() const {return ChannelId;}
	bool bChannelEnabled = false;
	int ChannelId;
	
private:

	struct FDivider
	{
		uint16 Reload; // timer load 
		uint16 Counter; // Countdown timer
	};
	
	struct FSweep
	{
		// Sweep information
		bool bSweepEnable;
		bool bSweepNegate;
		uint8 SweepDividerPeriod = 0;
		bool bSweepReload = false;
		uint8 SweepShiftCount = 0;
	};

	struct FEnvelope
	{
		bool bStart = false;
		FDivider Timer;
	};

	struct FSequencer
	{
		uint8 Duty = 0; // Which current Duty we are on. 0-3
		uint8 SequencePointer = 0; // values of 0-8
		FDivider Timer;
		uint16 TimerLow = 0;
		uint16 TimerHigh = 0;
	};

	bool bLengthCounterHalt;
	bool bConstantVol;
	uint16 Volume; // values of 0-15 with 0 being muted
	uint16 LengthCounter; // APU Length Counter
	
	FSweep Sweep;
	FEnvelope Envelope;
	FSequencer Sequencer;
	
	// Sequence lookup table - Thus it reads the sequence lookup table in the order 0, 7, 6, 5, 4, 3, 2, 1.
	// Use Duty/Sequence Pointer as lookup indexes SequenceLookupTable[Duty][SeqPointer]
	static constexpr uint8 SequenceLookupTable[4][8] = {
		{0, 0, 0, 0, 0, 0, 0, 1},
		{0, 0, 0, 0, 0, 0, 1, 1},
		{0, 0, 0, 0, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 0, 0}
	};

	// Output waveform is used to determine if we need to output a sound at this point.
	// Use Duty/result from SLT (The table above as indexes) 0,1,2,3,4,5,6,7
	static constexpr uint8 OutputWaveformTable [4][8] = {
		{0, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{1, 0, 0, 1, 1, 1, 1, 1}
	};

	bool GateCheck();
};
