// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC3.h"

NesMMC3::NesMMC3()
{
    ChrBankData.resize(6);
    PrgBankData.resize(2);
}

NesMMC3::~NesMMC3()
{
}

uint8 NesMMC3::Read(unsigned short Address)
{
    uint8 returnData = 0xFF;
    
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
    OpenBusData = Data;
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
        bool bIsEvenAddress = (Address % 2 == 0);
        // Bank select ($8000-$9FFE, even) / Bank data ($8001-$9FFF, odd)
        if(Address >= 0x8000 && Address <= 0x9FFF)
        {
            if(bIsEvenAddress) 
            {
                BankSelect.Write(Data);
            } 
            else 
            {
                uint8 BankRegister = BankSelect.GetBankRegister();
                /*
                    000: R0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)
                    001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)
                    010: R2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF)
                    011: R3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF)
                    100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)
                    101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)
                */
                if(BankRegister >= 0 && BankRegister <= 5)
                {
                    //R0 and R1 ignore the bottom bit, as the value written still counts banks in 1KB units but odd numbered banks can't be selected. 
                    if(BankRegister == 0 || BankRegister == 1) 
                    {
                        Data = Data & 0xFE;
                    }
                    ChrBankData.at(BankRegister) = Data;
                } 
                else if(BankRegister >= 6 && BankRegister <= 7)
                {
                    //R6 and R7 will ignore the top two bits, as the MMC3 has only 6 PRG ROM address lines. Some romhacks rely on an 8-bit extension of R6/7 for oversized PRG-ROM.
                    PrgBankData.at(BankRegister) = Data & 0x3F;
                }
            }
        }

        // Mirroring ($A000-$BFFE, even) / PRG RAM protect ($A001-$BFFF, odd)
        if(Address >= 0xA000 && Address <= 0xBFFF)
        {
            if(bIsEvenAddress) 
            {

            } 
            else 
            {
                
            }
        }

        // IRQ latch ($C000-$DFFE, even) / IRQ reload ($C001-$DFFF, odd)
        if(Address >= 0xC000 && Address <= 0xDFFF)
        {
            if(bIsEvenAddress) 
            {

            } 
            else 
            {
                
            }
        }

        // IRQ disable ($E000-$FFFE, even) / IRQ enable ($E001-$FFFF, odd)
        if(Address >= 0xE000 && Address <= 0xFFFF)
        {
            if(bIsEvenAddress) 
            {

            } 
            else 
            {
                
            }
        }
    }
}

uint8 NesMMC3::GetMirrorMode()
{
    return MirrorMode;
}