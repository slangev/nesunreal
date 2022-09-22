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

	struct FDivider
	{
		uint16 Reload; // timer load 
		uint16 Counter; // Countdown timer
	};

	struct FEnvelope
	{
		bool bStart = false;
		FDivider Timer;
		uint16 DelayLevelCounter;
		uint16 EnvelopeVol;
	};

	bool GateCheck();
	void EnvelopeTick();
	bool bLengthCounterHalt;
	bool bConstantVol;
	bool bModeFlag;

	uint16 ShiftRegister = 0x1; // On power-up, the shift register is loaded with the value 1. 
	uint16 Volume; // values of 0-15 with 0 being muted
	uint16 LengthCounter; // APU Length Counter
	uint16 LengthLoad;
	bool bChannelEnabled = false;
	FEnvelope Envelope;
	FDivider Timer;
	static constexpr uint NoisePeriodTable[] = 
	{
		4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
	};
};
