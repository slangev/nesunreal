// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC1.h"
#include "NesCPU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesMMC1,Log,All)

NesMMC1::NesMMC1()
{
}

NesMMC1::NesMMC1(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory, bool bBatteryBacked)
{
    this->PrgRomMemory = PrgRomMemory;
    this->PrgRamMemory = PrgRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
    ControlRegister = make_unique<NesMMC1ControlRegister>();
    ControlRegister->Write(0x0C);
    this->bBatteryBacked = bBatteryBacked;
}

uint8 NesMMC1::GetMirrorMode() 
{
    return ControlRegister->GetMirrorMode();
}

NesMMC1::~NesMMC1()
{
}

bool NesMMC1::GetIRQRequested()
{
    return false;
}

uint8 NesMMC1::Read(unsigned short Address) 
{
    uint8 returnData = 0xFF;

    // CHR ROM Address
    if (Address >= 0x0000 && Address <= 0x1FFF) 
    {
        uint8 BankNumber = 0x00;

        // switch 8 KB at a time
        if(ControlRegister->GetCHRROMBankMode() == 0) 
        {
            if(ChrRamMemory) 
            {
                returnData = ChrRamMemory->at((Address & 0x1FFF) & (ChrRamMemory->size() - 1));
            } 
            else 
            {
                BankNumber = ChrBank0Register & 0x1E; // Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
                uint32 Index = ((ROM_BANK_SIZE_8KB * BankNumber | (Address & 0x1FFF))) & (ChrRomMemory->size() - 1);
                returnData = ChrRomMemory->at(Index);
            }
        } 
        // switch two separate 4 KB banks
        else if(ControlRegister->GetCHRROMBankMode() == 1) 
        {
            if(ChrRamMemory) 
            {
                returnData = ChrRamMemory->at((Address & 0x0FFF) & (ChrRamMemory->size() - 1));
            }
            // PPU $0000-$0FFF: 4 KB switchable CHR bank 
            else 
            {
                if (Address >= 0x0000 && Address <= 0x0FFF) 
                {
                    BankNumber = ChrBank0Register & 0x1F; // Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
                    uint32 Index = ((ROM_BANK_SIZE_4KB * BankNumber | (Address & 0x0FFF))) & (ChrRomMemory->size() - 1);
                    returnData = ChrRomMemory->at(Index);
                }
                // PPU $1000-$1FFF: 4 KB switchable CHR bank
                else if (Address >= 0x1000 && Address <= 0x1FFF) 
                {
                    BankNumber = ChrBank1Register & 0x1F; // Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
                    uint32 Index = ((ROM_BANK_SIZE_4KB * BankNumber | (Address & 0x0FFF))) & (ChrRomMemory->size() - 1);
                    returnData = ChrRomMemory->at(Index);
                }
            }
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
        uint8 bankNumber = 0x00;
        // 32kb bank mode
        if(ControlRegister->GetPRGROMBankMode() == 0 || ControlRegister->GetPRGROMBankMode() == 1) 
        {
            bankNumber = PrgBankRegister & 0x1E; // Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
            uint32 Index = ((ROM_BANK_SIZE_32KB * bankNumber | (Address & 0x7FFF))) & (PrgRomMemory->size() - 1);
            returnData = PrgRomMemory->at(Index);
        } 
        // 16kb bank mode
        else 
        {
            bankNumber = PrgBankRegister & 0x1F; // Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
            uint32 Index = ((ROM_BANK_SIZE_16KB * bankNumber | (Address & 0x3FFF))) & (PrgRomMemory->size() - 1);

            // fix first bank at $8000 and switch 16 KB bank at $C000;
            if(ControlRegister->GetPRGROMBankMode() == 2) 
            {
                // CPU $8000-$BFFF: 16 KB PRG ROM bank, either switchable or fixed to the first bank
                if (Address >= 0x8000 && Address <= 0xBFFF) 
                {
                    Index = ((ROM_BANK_SIZE_16KB * 0 | (Address & 0x3FFF))) & (PrgRomMemory->size() - 1);
                    returnData = PrgRomMemory->at(Index);
                }
                // CPU $C000-$FFFF: 16 KB PRG ROM bank, either fixed to the last bank or switchable 
                else if (Address >= 0xC000 && Address <= 0xFFFF) 
                {
                    returnData = PrgRomMemory->at(Index);
                }
            }
            // fix last bank at $C000 and switch 16 KB bank at $8000 
            else if(ControlRegister->GetPRGROMBankMode() == 3) 
            {
                 // CPU $8000-$BFFF: 16 KB PRG ROM bank, either switchable or fixed to the first bank
                if (Address >= 0x8000 && Address <= 0xBFFF) 
                {
                    returnData = PrgRomMemory->at(Index);
                }
                // CPU $C000-$FFFF: 16 KB PRG ROM bank, either fixed to the last bank or switchable 
                else if (Address >= 0xC000 && Address <= 0xFFFF) 
                {
                    bankNumber = (PrgRomMemory->size() / ROM_BANK_SIZE_16KB) - 1; // last bank 
                    Index = ((ROM_BANK_SIZE_16KB * bankNumber | (Address & 0x3FFF))) & (PrgRomMemory->size() - 1);
                    returnData = PrgRomMemory->at(Index);
                }
            }
        }
    }
    
   
    return returnData & 0xFF;
}

void NesMMC1::Write(unsigned short Address, uint8 Data) 
{

    //ChrRam is located here if present.
    if(Address >= 0x0000 && Address < 0x2000) 
    {
        if(ChrRamMemory) 
        {
            uint8 bankNumber = 0x00; // Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
            uint32 Index = ((ROM_BANK_SIZE_4KB * bankNumber | (Address & 0x1FFF))) & (ChrRamMemory->size() - 1);
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
        LoadRegister = Data;
        // Writing a value with bit 7 set ($80 through $FF) to any address in $8000-$FFFF clears the shift register to its initial state.
        if(((LoadRegister >> 7) & 0x1) == 0x1) 
        {
            // Reset shift register and write Control with (Control OR $0C), locking PRG ROM at $C000-$FFFF to the last bank.
            ShiftRegister = 0x10;
            ControlRegister->Write(ControlRegister->Read() | 0x0C);
        } 
        // Only on the fifth write does the address matter, and even then, only bits 14 and 13 of the address matter
        else 
        {
            // Once a 1 is shifted into the last position, the SR is full.
            if((ShiftRegister & 0x1) == 0x1) 
            {
                ShiftRegister = ShiftRegister >> 1;
                uint8 bit = LoadRegister & 0x1;
                if(bit == 0x1) 
                {
                    ShiftRegister = FNesCPU::SetBit(4,ShiftRegister);
                }
                // only bits 14 and 13 of the address matter
                uint8 registerSelector = (Address >> 13) & 0x3;
                switch(registerSelector) 
                {
                    case RegisterSelector::ControlRegisterEnum:
                        ControlRegister->Write(ShiftRegister & 0x1F); //Only the 5 bits.
                        break;
                    case RegisterSelector::ChrBank0RegisterEnum:
                        ChrBank0Register = ShiftRegister & 0x1F; //Only the 5 bits.
                        break;
                    case RegisterSelector::ChrBank1RegisterEnum:
                        ChrBank1Register = ShiftRegister & 0x1F; //Only the 5 bits.
                        break;
                    case RegisterSelector::PrgBankRegisterEnum:
                        PrgBankRegister = ShiftRegister & 0x1F; //Only the 5 bits.
                        break;
                    default:
                        break;
                }
                // After the fifth write, the shift register is cleared automatically, so a write to the shift register with bit 7 on to reset it is not needed. 
                //UE_LOG(LogTemp,Warning, TEXT("ShiftRegister: %d registerSelector: %d ControlRegister: %d ChrBank0Register: %d ChrBank1Register: %d PrgBankRegister: %d"), ShiftRegister, registerSelector, ControlRegister->Read(),ChrBank0Register,ChrBank1Register,PrgBankRegister);
                ShiftRegister = 0x10; // SR gets cleared
            }
            // Write LSB from LR into SR. The Registers inside MMC1 are 5-bit wide. 
            else 
            {
                ShiftRegister = ShiftRegister >> 1;
                uint8 bit = LoadRegister & 0x1;
                if(bit == 0x1) {
                    ShiftRegister = FNesCPU::SetBit(4,ShiftRegister);
                }
                //UE_LOG(LogTemp,Warning, TEXT("ShiftRegister: %d"),ShiftRegister); 
            }
        }
    }
}