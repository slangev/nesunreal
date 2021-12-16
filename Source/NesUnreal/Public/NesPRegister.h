// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesPRegister
{
public:
	FNesPRegister();
	~FNesPRegister();
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
    void PSetState(const uint8 State) {P = State;};
    uint8 PStateWithBFlag(){return P;};
    uint8 PState() {return static_cast<uint8>(P & 0xEF);};
private:
	uint8 P = 0x24; //Although P starts at 0x34, the B flag doesn't exist in P. The B flag only exist in a copy of P that is pushed to the stack. This is the only time and place where the B flag actually exists: not in the status register itself, but in bit 4 of the copy that is written to the stack. 
};
