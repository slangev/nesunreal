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
    uint32 Index = 0;

    // CHR ROM Address
    /*
        PPU reads $0FD8: latch 0 is set to $FD for subsequent reads
        PPU reads $0FE8: latch 0 is set to $FE for subsequent reads
        PPU reads $1FD8 through $1FDF: latch 1 is set to $FD for subsequent reads
        PPU reads $1FE8 through $1FEF: latch 1 is set to $FE for subsequent reads
    */
    if (Address >= 0x0000 && Address <= 0x1FFF) 
    {
        uint BankNumber = 0;

        if(Address >= 0x0000 && Address <= 0x0FFF) 
        {
            /*
                CHR ROM $FD/0000 bank select
                Select 4 KB CHR ROM bank for PPU $0000-$0FFF
                used when latch 0 = $FD

                CHR ROM $FE/0000 bank select 
                Select 4 KB CHR ROM bank for PPU $0000-$0FFF
                used when latch 0 = $FE
            */
            if(Latch0 == 0xFD)
            {
                BankNumber = CHRROMFD0000;
            } 
            else
            {
                BankNumber = CHRROMFE0000;
            }
            Index = ((ROM_BANK_SIZE_4KB * BankNumber | (Address & 0x0FFF)));
        }
        else if(Address >= 0x1000 && Address <= 0x1FFF)
        {
            /*
                CHR ROM $FD/1000 bank select
                Select 4 KB CHR ROM bank for PPU $1000-$1FFF
                used when latch 1 = $FD

                CHR ROM $FE/1000 bank select 
                Select 4 KB CHR ROM bank for PPU $1000-$1FFF
                used when latch 1 = $FE
            */
            if(Latch1 == 0xFD)
            {
                BankNumber = CHRROMFD1000;
            } 
            else
            {
                BankNumber = CHRROMFE1000;
            }
            Index = ((ROM_BANK_SIZE_4KB * BankNumber | (Address & 0x0FFF)));
        }
        if(Address == 0x0FD8)
        {
            Latch0 = 0xFD;
        }
        else if(Address == 0x0FE8)
        {
            Latch0 = 0xFE;
        }
        else if(Address >= 0x1FD8 && Address <= 0x1FDF)
        {
            Latch1 = 0xFD;
        }
        else if(Address >= 0x1FE8 && Address <= 0x1FEF)
        {
            Latch1 = 0xFE;
        }
        if(ChrRamMemory) 
        {
            returnData = ChrRamMemory->at(Index) & (ChrRamMemory->size() - 1);
        }
        else
        {
            returnData = ChrRomMemory->at(Index) & (ChrRomMemory->size() - 1);
        }
    }

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    else if (Address >= 0x6000 && Address <= 0x7FFF) 
    {
        Index = (Address & 0x1FFF) & PrgRamMemory->size() - 1;
        returnData = PrgRamMemory->at(Index);
    }

    // PRG ROM Address
    else if(Address >= 0x8000 && Address <= 0xFFFF) 
    {
        // $8000-$9FFF: 8 KB switchable PRG ROM bank
        if(Address >= 0x8000 && Address <= 0x9FFF)
        {
            uint BankNumber = PRGROMBankSelect;
            Index = ((ROM_BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
        }
        // $A000-$FFFF: Three 8 KB PRG ROM banks, fixed to the last three banks
        else if(Address >= 0xA000 && Address <= 0xFFFF)
        {
            uint BankNumber = 0;
            if(Address >= 0xA000 && Address <= 0xBFFF)
            {
                BankNumber = (PrgRomMemory->size() / ROM_BANK_SIZE_8KB) - 3; // (-3) : the third last bank
                Index = ((ROM_BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }
            else if(Address >= 0xC000 && Address <= 0xDFFF)
            {
                BankNumber = (PrgRomMemory->size() / ROM_BANK_SIZE_8KB) - 2; // (-2) : the second last bank
                Index = ((ROM_BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }
            else if(Address >= 0xE000 && Address <= 0xFFFF)
            {
                BankNumber = (PrgRomMemory->size() / ROM_BANK_SIZE_8KB) - 1; // (-1) : the last bank
                Index = ((ROM_BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }
        }
        returnData = PrgRomMemory->at(Index);
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