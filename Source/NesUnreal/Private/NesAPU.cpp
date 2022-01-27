// Fill out your copyright notice in the Description page of Project Settings.


#include "NesAPU.h"
DEFINE_LOG_CATEGORY_STATIC(LogNesAPU,Log,All)

FNesApu::FNesApu()
{
	Pulse1 = std::make_unique<FNesPulse>();
	Pulse2 = std::make_unique<FNesPulse>();
	Mixer = std::make_unique<FNesAudioMixer>();
}

FNesApu::~FNesApu()
{
}

void FNesApu::Step(uint Cycle)
{

}

void FNesApu::Write(const unsigned short Address, uint8 Data)
{
	if(Address >= 0x4000 && Address <= 0x4003)
	{
		Pulse1->Write(Address, Data);
	}
	else if(Address >= 0x4004 && Address <= 0x4007)
	{
		Pulse2->Write(Address, Data);
	}
	else if(Address >= 0x4008 && Address <= 0x400B)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Triangle. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x400C && Address <= 0x400F)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Noise. Address: %d Data: %d"), Address, Data);
	}
	else if(Address >= 0x4010 && Address <= 0x4013)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to DMC. Address: %d Data: %d"), Address, Data);
	}
	else if(Address == 0x4015)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Status. Address: %d Data: %d"), Address, Data);
		bFiveStepMode = (Data & 0x80) == 0x80 ? true : false;
		bIRQInhibit = (Data & 0x40) == 0x40;
 	}
	else if(Address == 0x4017)
	{	
		UE_LOG(LogNesAPU,Warning,TEXT("Writing to Frame Counter. Address: %d Data: %d"), Address, Data);
	}
	else
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Bad Write. Address: %d Data: %d"), Address, Data);
	}
}

uint8 FNesApu::Read(const unsigned short Address)
{
	if(Address == 0x4015)
	{
		UE_LOG(LogNesAPU,Warning,TEXT("Reading to Status. Address: %d"), Address);
	}
	return 0;
}