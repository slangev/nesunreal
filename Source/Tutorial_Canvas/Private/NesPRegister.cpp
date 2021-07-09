// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPRegister.h"

NesPRegister::NesPRegister()
{
}

NesPRegister::~NesPRegister()
{
}

void NesPRegister::SetFlag(uint8 flag) {
    uint8 result = (uint8)(p | (1 << flag));
    p = result;
}

void NesPRegister::ResetFlag(uint8 flag) {
    uint8 result = (uint8)((p  & ~(1 << flag)));
    p = result;
}

uint8 NesPRegister::ReadFlag(uint8 flag) {
    uint8 result = (uint8)((p & (1 << flag)));
    result = (uint8)(result >> flag); 
    return result;
}
