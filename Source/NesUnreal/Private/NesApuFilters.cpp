// Fill out your copyright notice in the Description page of Project Settings.


#include "NesApuFilters.h"

FNesApuFilters::FNesApuFilters(): B0(0), B1(0), A1(0), PrevX(0), PrevY(0)
{
}

FNesApuFilters::~FNesApuFilters()
{
}

void FNesApuFilters::LowPassFilter(const float SampleHertz, const float CutOffFreq)
{
	float c = SampleHertz / PI / CutOffFreq;
	float ai = 1.0 / (1.0 + c);
	B0 = ai;
	B1 = ai;
	A1 = (1.0f - c) * ai;
	PrevX = 0.0f;
	PrevY = 0.0f;
}

void FNesApuFilters::HighPassFilter(const float SampleHertz, const float CutOffFreq)
{
	float c = SampleHertz / PI / CutOffFreq;
	float ai = 1.0 / (1.0 + c);
	B0 = c * ai;
	B1 = -c * ai;
	A1 = (1.0f - c) * ai;
	PrevX = 0.0f;
	PrevY = 0.0f;
}

float FNesApuFilters::Step(const float X)
{
	const float y = B0 * X + B1 * PrevX - A1 * PrevY;
	PrevY = y;
	PrevX = X;
	return y;
}
