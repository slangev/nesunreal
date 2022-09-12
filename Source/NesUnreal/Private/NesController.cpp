// Fill out your copyright notice in the Description page of Project Settings.


#include "NesController.h"

NesController::NesController()
{
    this->ControllerOneLatchedState = 0;
	this->ControllerOneCurrentState = 0;

	this->ControllerTwoLatchedState = 0;
	this->ControllerTwoCurrentState = 0;
}

NesController::~NesController()
{
}

void NesController::Write(unsigned Address, uint8 Data) 
{
    if(Address == 0x4016) 
    {
        if((Data & 0x1) == 0x0) {
            ControllerOneLatchedState = ControllerOneCurrentState;  
            bIsLatched = true;
        } else {
            bIsLatched = false;
        }
    }
}

uint8 NesController::Read(unsigned Address) 
{
    uint8 ReadBit = 0x0;
    if(Address == 0x4016) 
    {
        if(bIsLatched) {
            ReadBit = (ControllerOneLatchedState & 0x80) >> 7; // get MOST significant bit from latched state.
            ControllerOneLatchedState = ControllerOneLatchedState << 1;
        } else {
            ReadBit = (ControllerOneCurrentState & 0x80) >> 7; // get MOST significant bit from current state. 
        }
    }
    return ReadBit | 0x40;
}

void NesController::SetKey(uint8 Key) 
{
   ControllerOneCurrentState = static_cast<uint8>(ControllerOneCurrentState | (1 << Key));
}

void NesController::ResetKey(uint8 Key)
{
    ControllerOneCurrentState = static_cast<uint8>((ControllerOneCurrentState & ~(1 << Key)));
}