// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC3.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesMMC3,Log,All)

NesMMC3::NesMMC3()
{
    BankSelect = std::make_unique<NesMMC3BankSelectRegister>();
    ChrBankData.resize(6);
    PrgBankData.resize(2);
}

NesMMC3::NesMMC3(std::shared_ptr<std::vector<uint8>> PrgRomMemory, std::shared_ptr<std::vector<uint8>> PrgRamMemory, std::shared_ptr<std::vector<uint8>> ChrRomMemory, std::shared_ptr<std::vector<uint8>> ChrRamMemory, bool bBatteryBacked)
{
    this->PrgRomMemory = PrgRomMemory;
    this->PrgRamMemory = PrgRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
    this->bBatteryBacked = bBatteryBacked;
    BankSelect = std::make_unique<NesMMC3BankSelectRegister>();
    ChrBankData.resize(6);
    PrgBankData.resize(2);
}

NesMMC3::~NesMMC3()
{
}

bool NesMMC3::GetIRQRequested()
{
    return bIRQ;
}

uint8 NesMMC3::Read(unsigned short Address)
{
    uint8 returnData = OpenBusData;
    
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
        if(BankSelect->GetCHRROMBankMode() == 0)
        {
            uint BankNumber = 0;
            uint32 Index = 0;
            // 000: R0: Select 2 KB CHR bank at PPU $0000-$07FF
            if(Address >= 0x0000 && Address <= 0x07FF)
            {
                BankNumber = ChrBankData.at(0);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x07FF))) & (ChrRomMemory->size() - 1);
            }
            // 001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF
            else if(Address >= 0x0800 && Address <= 0x0FFF)
            {
                BankNumber = ChrBankData.at(1);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x07FF))) & (ChrRomMemory->size() - 1);
            }
            // 010: R2: Select 1 KB CHR bank at PPU $1000-$13FF
            else if(Address >= 0x1000 && Address <= 0x13FF)
            {
                BankNumber = ChrBankData.at(2);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 011: R3: Select 1 KB CHR bank at PPU $1400-$17FF
            else if(Address >= 0x1400 && Address <= 0x17FF)
            {
                BankNumber = ChrBankData.at(3);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF
            else if(Address >= 0x1800 && Address <= 0x1BFF)
            {
                BankNumber = ChrBankData.at(4);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF
            else if(Address >= 0x1C00 && Address <= 0x1FFF)
            {
                BankNumber = ChrBankData.at(5);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }

            returnData = ChrRomMemory->at(Index);
        } 
        else if(BankSelect->GetCHRROMBankMode() == 1)
        {
            uint BankNumber = 0;
            uint32 Index = 0;
            // 010: R2: Select 1 KB CHR bank at PPU $0000-$03FF
            if(Address >= 0x0000 && Address <= 0x03FF)
            {
                BankNumber = ChrBankData.at(2);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 011: R3: Select 1 KB CHR bank at PPU $0400-$07FF
            else if(Address >= 0x0400 && Address <= 0x07FF)
            {
                BankNumber = ChrBankData.at(3);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)
            else if(Address >= 0x0800 && Address <= 0x0BFF)
            {
                BankNumber = ChrBankData.at(4);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)
            else if(Address >= 0x0C00 && Address <= 0x0FFF)
            {
                BankNumber = ChrBankData.at(5);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x03FF))) & (ChrRomMemory->size() - 1);
            }
            // 000: R0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)
            else if(Address >= 0x1000 && Address <= 0x17FF)
            {
                BankNumber = ChrBankData.at(0);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x07FF))) & (ChrRomMemory->size() - 1);
            }
            // 001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)
            else if(Address >= 0x1800 && Address <= 0x1FFF)
            {
                BankNumber = ChrBankData.at(1);
                Index = ((BANK_SIZE_1KB * BankNumber | (Address & 0x07FF))) & (ChrRomMemory->size() - 1);
            }
            returnData = ChrRomMemory->at(Index);
        }
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
        uint BankNumber = 0;
        uint32 Index = 0;
        if(BankSelect->GetPRGROMBankMode() == 0)
        {
            if(Address >= 0x8000 && Address <= 0x9FFF)
            {
                BankNumber = PrgBankData.at(0); //R6
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }

            else if(Address >= 0xA000 && Address <= 0xBFFF)
            {
                BankNumber = PrgBankData.at(1); //R7
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }

            else if(Address >= 0xC000 && Address <= 0xDFFF)
            {
                BankNumber = (PrgRomMemory->size() / BANK_SIZE_8KB) - 2; //(-2)
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }

            else if(Address >= 0xE000 && Address <= 0xFFFF)
            {
                BankNumber = (PrgRomMemory->size() / BANK_SIZE_8KB) - 1; //(-1)
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }
        } 
        else if(BankSelect->GetPRGROMBankMode() == 1)
        {
            if(Address >= 0x8000 && Address <= 0x9FFF)
            {
                BankNumber = (PrgRomMemory->size() / BANK_SIZE_8KB) - 2; //(-2)
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }   

            else if(Address >= 0xA000 && Address <= 0xBFFF)
            {
                BankNumber = PrgBankData.at(1); //R7
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }

            else if(Address >= 0xC000 && Address <= 0xDFFF)
            {
                BankNumber = PrgBankData.at(0); //R6
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }

            else if(Address >= 0xE000 && Address <= 0xFFFF)
            {
                BankNumber = (PrgRomMemory->size() / BANK_SIZE_8KB) - 1; //(-1)
                Index = ((BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (PrgRomMemory->size() - 1);
            }
        }
        returnData = PrgRomMemory->at(Index);
    }
    return returnData & 0xFF;
}

void NesMMC3::Write(unsigned short Address, uint8 Data)
{
    OpenBusData = Data;

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    if (Address >= 0x6000 && Address <= 0x7FFF) 
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
                BankSelect->Write(Data);
            } 
            else 
            {
                uint8 BankRegister = BankSelect->GetBankRegister();
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
                    PrgBankData.at(BankRegister-6) = Data & 0x3F;
                }
            }
        }

        // Mirroring ($A000-$BFFE, even) / PRG RAM protect ($A001-$BFFF, odd)
        if(Address >= 0xA000 && Address <= 0xBFFF)
        {
            if(bIsEvenAddress) 
            {
                // (0: vertical; 1: horizontal)
                MirrorMode = Data & 0x1;
            } 
            else 
            {
                PrgRamProtect = Data;
            }
        }

        // IRQ latch ($C000-$DFFE, even) / IRQ reload ($C001-$DFFF, odd)
        if(Address >= 0xC000 && Address <= 0xDFFF)
        {
            if(bIsEvenAddress) 
            {
                IRQLatch = Data;
            } 
            else 
            {
                IRQCounter = 0;
                bIRQReloadRequest = true;
            }
        }

        // IRQ disable ($E000-$FFFE, even) / IRQ enable ($E001-$FFFF, odd)
        if(Address >= 0xE000 && Address <= 0xFFFF)
        {
            if(bIsEvenAddress) 
            {
                bIsIRQEnabled = false;
            } 
            else 
            {
                bIsIRQEnabled = true;
            }
        }
    }
}

uint8 NesMMC3::GetMirrorMode()
{
    return MirrorMode+2; // The caller(NesPPUMMU) is expecting vertical to be 2 or horizontal to by 3
}