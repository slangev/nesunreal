// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "NesPulse.h"
#include "FNesAudioMixer.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesApu
{
public:
	FNesApu();
	~FNesApu();
	void Step(uint Cycle);
	void Write(const unsigned short Address, uint8 Data);
	uint8 Read(unsigned short Address);
	int Count = 0;
	bool bFiveStepMode = false; // 5-Step Sequence (bit 7 of $4017 set)
	bool bIRQInhibit = false; // Interrupt inhibit flag. If set, the frame interrupt flag is cleared, otherwise it is unaffected.

	// Length table constant
	static constexpr uint LengthTable[] = {
		10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30}
	;
protected:
	std::unique_ptr<FNesPulse> Pulse1;
	std::unique_ptr<FNesPulse> Pulse2;
	std::unique_ptr<FNesAudioMixer> Mixer;
};
