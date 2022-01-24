// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"

/**
 * https://wiki.nesdev.org/w/index.php/APU_Mixer
 */
class NESUNREAL_API FNesAudioMixer final
{
public:
	FNesAudioMixer();
	~FNesAudioMixer();

	static float LinearApproximationPulseOut(uint8 Pulse1, uint8 Pulse2);
	static float LinearApproximationTndOut(uint8 Triangle, uint8 Noise, uint8 Dmc);
	float LookupPulseTable(uint8 Pulse1, uint8 Pulse2) const;
	float LookupTndTable(uint8 Triangle, uint8 Noise, uint8 Dmc) const;
	static float FasterLinearApproximationPulseOut(uint8 Pulse1, uint8 Pulse2);
	static float FasterLinearApproximationTndOut(uint8 Triangle, uint8 Noise, uint8 Dmc);
private:
	void GenerateLookupTable();
	std::vector<float> PulseTable;
	std::vector<float> TndTable;

	int PulseTableSize = 31;
	int TndTableSize = 203;
};
