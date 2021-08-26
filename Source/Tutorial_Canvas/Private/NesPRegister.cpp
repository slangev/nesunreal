// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPRegister.h"

NesPRegister::NesPRegister()
{
}

NesPRegister::~NesPRegister()
{
}

void NesPRegister::SetFlag(const uint8 Flag) {
    const uint8 Result = static_cast<uint8>(p | (1 << Flag));
    p = Result;
}

void NesPRegister::ResetFlag(const uint8 Flag) {
    const uint8 Result = static_cast<uint8>((p & ~(1 << Flag)));
    p = Result;
}

uint8 NesPRegister::ReadFlag(const uint8 Flag) {
    uint8 Result = static_cast<uint8>((p & (1 << Flag)));
    Result = static_cast<uint8>(Result >> Flag); 
    return Result;
}
