// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TUTORIAL_CANVAS_API NesPRegister
{
public:
	NesPRegister();
	~NesPRegister();
	const uint8 NFlag = 7; //Negative flag
    const uint8 VFlag = 6; //Overflow flag
    const uint8 XFlag = 5; //Unused flag (No, really don't use it.)
    const uint8 BFlag = 4; //Break flag
    const uint8 DFlag = 3; //Decimal flag
    const uint8 IFlag = 2; //Interrupt flag
    const uint8 ZFlag = 1; //Zero flag
	const uint8 CFlag = 0; //Carry Flag
private:
	uint8 p = 0x34;
};
