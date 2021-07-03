// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
using namespace std;

/**
 * 
 */
class TUTORIAL_CANVAS_API NesHeader
{
public:
	NesHeader();
	~NesHeader();
	string nes = "";
	uint8 oneA = 0;
	uint8 numberOfPROROMS = 0;
	uint8 numberOfCHRROMS = 0;
	uint8 ROMControlByteOne = 0;
	uint8 ROMControlByteTwo = 0;
	uint8 numberOfRamBanks = 0;
	string futureUsage = ""; // should be 7 zeros
	int mapper = 0;
	uint8 mirroring = 0; //0 = horizontal, 1 = vertical, and 2 = four screen mirroring
};
