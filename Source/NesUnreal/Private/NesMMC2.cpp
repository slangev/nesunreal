// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC2.h"

NesMMC2::NesMMC2()
{
}

NesMMC2::NesMMC2(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory, bool bBatteryBacked)
{
    this->PrgRomMemory = PrgRomMemory;
    this->PrgRamMemory = PrgRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
    this->bBatteryBacked = bBatteryBacked;
}

NesMMC2::~NesMMC2()
{
}

uint8 NesMMC2::GetMirrorMode() 
{
    return 0;
}

bool NesMMC2::GetIRQRequested()
{
    return false;
}

uint8 NesMMC2::Read(unsigned short Address) 
{
    uint8 returnData = 0xFF;

    // CHR ROM Address
    if (Address >= 0x0000 && Address <= 0x1FFF) 
    {

    }

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {
        uint Index = (Address & 0x1FFF) & PrgRamMemory->size() - 1;
        returnData = PrgRamMemory->at(Index);

    }

    // PRG ROM Address
    else if(Address >= 0x8000 && Address <= 0xFFFF) 
    {
        
        
    }
    
   
    return returnData & 0xFF;
}

void NesMMC2::Write(unsigned short Address, uint8 Data) 
{
     //ChrRam is located here if present.
    if(Address >= 0x0000 && Address < 0x2000) 
    {
        if(ChrRamMemory) 
        {
            uint8 bankNumber = 0x00;
            uint32 Index = ((ROM_BANK_SIZE_8KB * bankNumber | (Address & 0x1FFF))) & (ChrRamMemory->size() - 1);
            ChrRamMemory->at(Index) = Data;
        }
    }
    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {
        // MMC1B and later: PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
        uint8 bit = FNesCPU::GetBit(5,PrgBankRegister);
        PrgRamMemory->at(Address & 0x1FFF) = Data;
    } 
    // CPU $8000-$FFFF is connected to a common shift register
    else if(Address >= 0x8000 && Address <= 0xFFFF) 
    {
       
    }
}

void NesMMC2::UpdateIRQCounter()
{
    // Do nothing
}