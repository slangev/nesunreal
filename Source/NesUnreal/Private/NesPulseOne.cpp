// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPulseOne.h"

FNesPulseOne::FNesPulseOne(): bLengthCounterHalt(false), bConstantVol(false), Volume(0)
{
}

FNesPulseOne::~FNesPulseOne()
{
	
}

void FNesPulseOne::Tick()
{
	UE_LOG(LogTemp, Warning, TEXT("OVERRIDE"));
}

void FNesPulseOne::HalfFrameTick()
{
}

void FNesPulseOne::QuarterFrameTick()
{
}

void FNesPulseOne::WriteData(const int Address, int Data)
{
	switch(Address)
	{
	case 0x4000:
		{
			Volume = Data & 0xF;
			bConstantVol = (Data & 0x10) == 0x10;
			bLengthCounterHalt = (Data & 0x20) == 0x20;
			Duty = (Data >> 6) & 0x3;
			break;
		}
	default: ;
	}
}

void FNesPulseOne::Enabled(bool bEnabled)
{
}

int FNesPulseOne::GetOutputVol()
{
	return 0;
}

bool FNesPulseOne::LengthAboveZero()
{
	return false;
}
