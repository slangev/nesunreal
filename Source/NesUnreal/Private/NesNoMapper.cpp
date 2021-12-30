// Fill out your copyright notice in the Description page of Project Settings.


#include "NesNoMapper.h"

NesNoMapper::NesNoMapper()
{
}

NesNoMapper::NesNoMapper(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PRGRomMemory = PRGRomMemory;
    this->PRGRamMemory = PRGRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
}

NesNoMapper::~NesNoMapper()
{
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
        returnData = PRGRamMemory->at(Address - 0x6000);
    }
    // Read from PRGROM
    else if (Address >= 0x8000 && Address <= 0xFFFF){
        uint32 index = (Address - 0x8000) & (PRGRomMemory->size() - 1);
        returnData = PRGRomMemory->at(index);
    }
    return returnData & 0xFF;
}

void NesNoMapper::Write(unsigned short Address, uint8 Data) {
    // Cart Ram/SRAM
    if (Address >= 0x6000 && Address <= 0x7FFF){
        PRGRamMemory->at(Address - 0x6000) = Data;
    }
}