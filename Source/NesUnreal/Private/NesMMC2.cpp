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
    return MirrorMode+2; // The caller(NesPPUMMU) is expecting vertical to be 2 or horizontal to by 3
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
        if(Address >= 0x0000 && Address <= 0x0FFF) 
        {

        }
        else if(Address >= 0x1000 && Address <= 0x1FFF)
        {
            
        }
    }

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {

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
    // CPU $6000-$7FFF: 8 KB PRG RAM bank (PlayChoice version only; contains a 6264 and 74139)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {
        PrgRamMemory->at(Address & 0x1FFF) = Data;
    } 
    // CPU $8000-$9FFF 8 KB switchable PRG ROM bank
    else if(Address >= 0x8000 && Address <= 0x9FFF) 
    {
       
    }
    // CPU $A000-$FFFF: Three 8 KB PRG ROM banks, fixed to the last three banks
    else if(Address >= 0x8000 && Address <= 0xFFFF) 
    {
        // PRG ROM bank select ($A000-$AFFF)
        if(Address >= 0xA000 && Address <= 0xAFFF)
        {
            PRGROMBankSelect = Data & 0xF;
        }
        // CHR ROM $FD/0000 bank select ($B000-$BFFF)
        else if(Address >= 0xB000 && Address <= 0xBFFF)
        {
            CHRROMFD0000 = Data & 0x1F;
        }
        // CHR ROM $FE/0000 bank select ($C000-$CFFF)
        else if(Address >= 0xC000 && Address <= 0xCFFF)
        {
            CHRROMFE0000 = Data & 0x1F;
        }
        // CHR ROM $FD/1000 bank select ($D000-$DFFF)
        else if(Address >= 0xD000 && Address <= 0xDFFF)
        {
            CHRROMFD1000 = Data & 0x1F;
        }
        // CHR ROM $FE/1000 bank select ($E000-$EFFF)
        else if(Address >= 0xE000 && Address <= 0xEFFF)
        {
            CHRROMFE1000 = Data & 0x1F;
        }
        // Mirroring ($F000-$FFFF)
        else if(Address >= 0xF000 && Address <= 0xFFFF)
        {
            MirrorMode = Data & 0x1;
        }
    }
}

void NesMMC2::UpdateIRQCounter()
{
    // Do nothing
}