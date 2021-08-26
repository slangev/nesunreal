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
    void SetFlag(uint8 Flag);
    void ResetFlag(uint8 Flag);
    uint8 ReadFlag(uint8 Flag);
    void pSetState(uint8 state) {p = state;};
    uint8 pStateWithBFlag(){return p;};
    uint8 pState() {return (uint8)(p & 0xEF);};
private:
	uint8 p = 0x34;
};
