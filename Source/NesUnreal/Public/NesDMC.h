// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesCart.h"
#include "NesSoundChannel.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesDMC final: FNesSoundChannel
{
public:
	NesDMC();
	~NesDMC();
	virtual void Tick() override;
	virtual void HalfFrameTick() override;
	virtual void QuarterFrameTick() override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual void Enabled(bool bEnabled) override;
	virtual void LengthTick() override;
	virtual int GetOutputVol() override;
	virtual bool LengthAboveZero() override;
	void AttachCart(shared_ptr<NesCart> Cart);
	
private:
	struct FDivider
	{
		uint16 Reload; // timer load 
		uint16 Counter; // Countdown timer
	};

	// RateIndexTable
	static constexpr uint RateIndexTable[] = 
	{
		428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
	};

	uint8 RateIndex = 0;
	FDivider Timer;
	bool bLoopFlag = false;
	bool bIRQEnabled = false;

	// Output unit
	bool bSilence = false;
	uint8 Output = 0;
	uint8 ShiftRegister = 0;
	uint8 BitRemaining = 0;

	uint16 SampleAddress = 0x0000;
	uint16 SampleLength = 0x0000;
	uint8 SampleBuffer = 0x00;

	std::shared_ptr<NesCart> M_Cart;
};
