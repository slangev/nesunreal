// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesApuFilters
{
public:
	FNesApuFilters();
	~FNesApuFilters();
	float B0;
	float B1;
	float A1;
	float PrevX;
	float PrevY;

	void LowPassFilter(float SampleHertz, float CutOffFreq);
	void HighPassFilter(float SampleHertz, float CutOffFreq);
	float Step(float X);
	
private:
	const float Pi = 3.14159265358979323846f;
};
