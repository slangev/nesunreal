// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPRegister.h"

FNesPRegister::FNesPRegister()
{
}

FNesPRegister::~FNesPRegister()
{
}

void FNesPRegister::SetFlag(const uint8 Flag) {
    const uint8 Result = static_cast<uint8>(P | (1 << Flag));
    P = Result;
}

void FNesPRegister::ResetFlag(const uint8 Flag) {
    const uint8 Result = static_cast<uint8>((P & ~(1 << Flag)));
    P = Result;
}

uint8 FNesPRegister::ReadFlag(const uint8 Flag) {
    uint8 Result = static_cast<uint8>((P & (1 << Flag)));
    Result = static_cast<uint8>(Result >> Flag); 
    return Result;
}
