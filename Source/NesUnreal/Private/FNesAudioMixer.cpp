// Fill out your copyright notice in the Description page of Project Settings.


#include "FNesAudioMixer.h"

FNesAudioMixer::FNesAudioMixer()
{
	GenerateLookupTable();
}

FNesAudioMixer::~FNesAudioMixer()
{
}

float FNesAudioMixer::LinearApproximationPulseOut(const uint8 Pulse1, const uint8 Pulse2)
{
	if(Pulse1 == 0 && Pulse2 == 0)
	{
		return 0;
	}
	return 95.88f / (8128.0f / (Pulse1 + Pulse2)) + 100.0f;
}

float FNesAudioMixer::LinearApproximationTndOut(const uint8 Triangle, const uint8 Noise, uint8 Dmc)
{
	//When the values for one of the groups are all zero, the result for that group should be treated as zero rather than undefined due to the division by 0 that otherwise results. 
	const float TriangleGroup = (Triangle == 0) ? 0 : (Triangle / 8227.0f);
	const float NoiseGroup = (Noise == 0) ? 0 : (Noise / 12241.0f);
	const float DmcGroup = (Dmc == 0) ? 0 : (Dmc / 22648.0f);
	const float DenominatorGroup = (TriangleGroup == 0 && NoiseGroup == 0 && DmcGroup == 0) ? 0 : TriangleGroup + NoiseGroup + DmcGroup;
	return 159.79f / (1.0f / DenominatorGroup + 100.0f);
}

float FNesAudioMixer::LookupPulseTable(const uint8 Pulse1, const uint8 Pulse2) const
{
	return PulseTable.at(Pulse1+Pulse2);
}

float FNesAudioMixer::LookupTndTable(const uint8 Triangle, const uint8 Noise, const uint8 Dmc) const
{
	return TndTable.at(3 * Triangle + 2 * Noise + Dmc);
}

float FNesAudioMixer::FasterLinearApproximationPulseOut(const uint8 Pulse1, const uint8 Pulse2)
{
	return 0.00752 * (Pulse1 + Pulse2);
}

float FNesAudioMixer::FasterLinearApproximationTndOut(const uint8 Triangle, const uint8 Noise, const uint8 Dmc)
{
	return 0.00851 * Triangle + 0.00494 * Noise + 0.00335 * Dmc;
}

void FNesAudioMixer::GenerateLookupTable()
{
	for(float i = 0.0f; i < PulseTableSize; i++)
	{
		PulseTable.push_back(95.52f / (8128.0 / i + 100.0f));
	}

	for(float i = 0.0f; i < TndTableSize; i++)
	{
		TndTable.push_back(163.67 / (24329.0 / i + 100.0f));
	}
}
