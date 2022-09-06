// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC1ControlRegister.h"

NesMMC1ControlRegister::NesMMC1ControlRegister()
{
}

NesMMC1ControlRegister::~NesMMC1ControlRegister()
{
}

uint8 NesMMC1ControlRegister::Read() {
    return M_Data & 0x1F;
}

void NesMMC1ControlRegister::Write(uint8 Data) {
    M_Data = Data;
    M_MirrorMode = Data & 0x03;
    M_PRGROMBankMode = Data & 0x0C;
    M_CHRROMBankMode = Data & 0x10;
}
