// Fill out your copyright notice in the Description page of Project Settings.


#include "NesCNROM.h"

NesCNROM::NesCNROM()
{
}

NesCNROM::NesCNROM(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PrgRomMemory = PrgRomMemory;
    this->PrgRamMemory = PrgRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
}

NesCNROM::~NesCNROM()
{
}

uint8 NesCNROM::GetMirrorMode() 
{
    return 0xFF; // The mode is determine by the header
}

uint8 NesCNROM::Read(unsigned short Address) 
{
    uint8 returnData = 0xFF;

    // Read from CHRROM/RAM
    if (Address >= 0x0000 && Address < 0x2000) 
    {
        // if we have chrRam, read from it.
        if(ChrRamMemory) 
        {
            returnData = ChrRamMemory->at(Address);
        } 
        else 
        {
            uint32 index = ((ROM_BANK_SIZE_8KB * (ChrBankSelect & 0x3) | (Address & 0x7FFF))) & (ChrRomMemory->size() - 1);
            returnData = ChrRomMemory->at(index);
        }
    }
    // Read from SRAM 
    else if (Address >= 0x6000 && Address <= 0x7FFF)
    {
        returnData = PrgRamMemory->at(Address & 0x6000);
    }
    // Read from PRGROM
    else if (Address >= 0x8000 && Address <= 0xFFFF)
    {
        uint32 index = (Address - 0x8000) & (PrgRomMemory->size() - 1);
        returnData = PrgRomMemory->at(index);
    }
    return returnData & 0xFF;
}

void NesCNROM::Write(unsigned short Address, uint8 Data) 
{
    // Cart CHRRom/CHRRAM
    if (Address >= 0x0000 && Address < 0x2000) 
    {
        // if we have chrRam, write to it.
        if(ChrRamMemory) 
        {
            ChrRamMemory->at(Address) = Data;
        } 
        else 
        {
            // This should never happen.
            uint32 index = ((ROM_BANK_SIZE_8KB * (ChrBankSelect & 0x3) | (Address & 0x7FFF))) & (ChrRomMemory->size() - 1);
            ChrRomMemory->at(Address) = Data;
        }
    }
    // Cart PRGRAM
    else if (Address >= 0x6000 && Address <= 0x7FFF)
    {
        PrgRamMemory->at(Address & 0x1FFF) = Data;
    }
    // CNROM only implements the lowest 2 bits, capping it at 32 KiB CHR. Other boards may implement 4 or more bits for larger CHR.  
    else if (Address >= 0x8000 && Address <= 0xFFFF) 
    {
        ChrBankSelect = Data;
    }
}

bool NesCNROM::GetIRQRequested()
{
    return false;
}

void NesCNROM::UpdateIRQCounter()
{
    // Do nothing
}