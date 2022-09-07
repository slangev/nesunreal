// Fill out your copyright notice in the Description page of Project Settings.


#include "NesNoMapper.h"

NesNoMapper::NesNoMapper()
{
}

NesNoMapper::NesNoMapper(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PrgRomMemory = PrgRomMemory;
    this->PrgRamMemory = PrgRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
}

NesNoMapper::~NesNoMapper()
{
}

uint8 NesNoMapper::GetMirrorMode() {
    return 0xFF; // The mode is determine by the header
}

uint8 NesNoMapper::Read(unsigned short Address) {
    uint8 returnData = 0xFF;

    // Read from CHRROM/RAM
    if (Address >= 0x0000 && Address < 0x2000) {
        // if we have chrRam, read from it.
        if(ChrRamMemory) {
            returnData = ChrRamMemory->at(Address);
        } else {
            returnData = ChrRomMemory->at(Address);
        }
    }
    // Read from SRAM 
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        returnData = PrgRamMemory->at(Address - 0x6000);
    }
    // Read from PRGROM
    else if (Address >= 0x8000 && Address <= 0xFFFF){
        uint32 index = (Address - 0x8000) & (PrgRomMemory->size() - 1);
        returnData = PrgRomMemory->at(index);
    }
    return returnData & 0xFF;
}

void NesNoMapper::Write(unsigned short Address, uint8 Data) {
    // Cart CHRRom/CHRRAM
    if (Address >= 0x0000 && Address < 0x2000) {
        // if we have chrRam, read from it.
        if(ChrRamMemory) {
            ChrRamMemory->at(Address) = Data;
        } else {
            ChrRomMemory->at(Address) = Data;
        }
    }
    // Cart PRGRAM
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        PrgRamMemory->at(Address - 0x6000) = Data;
    }
}