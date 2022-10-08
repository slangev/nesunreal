// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC3.h"

NesMMC3::NesMMC3()
{
}

NesMMC3::~NesMMC3()
{
}

uint8 NesMMC3::Read(unsigned short Address)
{
    //TODO: Openbus
    uint8 returnData = 0x0;
    
    // CHR ROM Address
    /*
        PPU $0000-$07FF (or $1000-$17FF): 2 KB switchable CHR bank
        PPU $0800-$0FFF (or $1800-$1FFF): 2 KB switchable CHR bank
        PPU $1000-$13FF (or $0000-$03FF): 1 KB switchable CHR bank
        PPU $1400-$17FF (or $0400-$07FF): 1 KB switchable CHR bank
        PPU $1800-$1BFF (or $0800-$0BFF): 1 KB switchable CHR bank
        PPU $1C00-$1FFF (or $0C00-$0FFF): 1 KB switchable CHR bank
    */
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
    return 0;
}

void NesMMC3::Write(unsigned short Address, uint8 Data)
{
    
    // CHR ROM Address
    if (Address >= 0x0000 && Address <= 0x1FFF) 
    {
       
    }

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {
        PrgRamMemory->at(Address & 0x1FFF) = Data;
    }

    // PRG ROM Address
    else if(Address >= 0x8000 && Address <= 0xFFFF) 
    {
        
    }
}

uint8 NesMMC3::GetMirrorMode()
{
    return 0;
}