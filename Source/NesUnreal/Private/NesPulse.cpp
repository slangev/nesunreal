// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPulse.h"

FNesPulse::FNesPulse()
{
}

FNesPulse::~FNesPulse()
{
	
}

void FNesPulse::Tick()
{
	UE_LOG(LogTemp, Warning, TEXT("OVERRIDE"));
}

void FNesPulse::HalfFrameTick()
{
}

void FNesPulse::QuarterFrameTick()
{
}

void FNesPulse::WriteData(int Address, int Data)
{
}

void FNesPulse::Enabled(bool bEnabled)
{
}

int FNesPulse::GetOutputVol()
{
	return 0;
}

bool FNesPulse::LengthAboveZero()
{
	return false;
}
