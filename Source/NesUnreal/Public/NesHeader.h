// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
using namespace std;

/**
 * 
 */
class NESUNREAL_API FNesHeader
{
public:
	FNesHeader();
	~FNesHeader();
	string Nes = "";
	FString GameTitle = "";
	uint8 OneA = 0;
	uint8 NumberOfPrgRoms = 0;
	uint8 NumberOfChrRoms = 0;
	uint8 RomControlByteOne = 0;
	uint8 RomControlByteTwo = 0;
	uint8 NumberOfRamBanks = 0;
	string FutureUsage = ""; // should be 7 zeros
	int Mapper = 0;
	uint8 Mirroring = 0; //0 = horizontal, 1 = vertical, and 2 = four screen mirroring
};
