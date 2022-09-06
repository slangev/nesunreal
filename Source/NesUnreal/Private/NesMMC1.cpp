// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC1.h"
#include "NesCPU.h"

NesMMC1::NesMMC1()
{
}

NesMMC1::NesMMC1(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PRGRomMemory = PRGRomMemory;
    this->PRGRamMemory = PRGRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
    ControlRegister = make_unique<NesMMC1ControlRegister>();
}

uint8 NesMMC1::GetMirrorMode() {
    return ControlRegister->GetMirrorMode();
}

NesMMC1::~NesMMC1()
{
}
// uint32 index = (int)(((rom_lower | (uint)((Address & 0x3fff))) & (PRGRamMemory->size() - 1)));
uint8 NesMMC1::Read(unsigned short Address) {
    uint8 returnData = 0xFF;

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    if (Address >= 0x6000 && Address <= 0x7FFF) {
        uint Index = (Address & 0x1FFF) & PRGRamMemory->size() - 1;
        returnData = PRGRamMemory->at(Index);
    }
    // CPU $8000-$BFFF: 16 KB PRG ROM bank, either switchable or fixed to the first bank
    else if (Address >= 0x8000 && Address <= 0xBFFF) {
        
    }
     // CPU $C000-$FFFF: 16 KB PRG ROM bank, either fixed to the last bank or switchable 
    else if (Address >= 0x8000 && Address <= 0xBFFF) {
       
    }
    // PPU $0000-$0FFF: 4 KB switchable CHR bank
    else if (Address >= 0x0000 && Address <= 0x0FFF) {
        
    }
    // PPU $1000-$1FFF: 4 KB switchable CHR bank
    else if (Address >= 0x1000 && Address <= 0x1FFF) {
        
    }
    return returnData & 0xFF;
}

void NesMMC1::Write(unsigned short Address, uint8 Data) {
    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    if (Address >= 0x6000 && Address <= 0x7FFF) {
        // MMC1B and later: PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
        uint8 bit = FNesCPU::GetBit(5,PrgBankRegister);

        PRGRamMemory->at(Address - 0x6000) = Data;
    } 
    // PPU $0000-$0FFF: 4 KB switchable CHR bank
    else if (Address >= 0x0000 && Address <= 0x0FFF) {
        
    }
    // PPU $1000-$1FFF: 4 KB switchable CHR bank
    else if (Address >= 0x1000 && Address <= 0x1FFF) {
        
    }
    // CPU $8000-$FFFF is connected to a common shift register
    else if(Address >= 0x8000 && Address <= 0xFFFF) {
        LoadRegister = Data;
        // Writing a value with bit 7 set ($80 through $FF) to any address in $8000-$FFFF clears the shift register to its initial state.
        if(((LoadRegister >> 7) & 0x1) == 0x1) {
            // Reset shift register and write Control with (Control OR $0C), locking PRG ROM at $C000-$FFFF to the last bank.
            ShiftRegister = 0x10;
            ControlRegister->Write(ControlRegister->Read() | 0x0C);
        } 
        // Only on the fifth write does the address matter, and even then, only bits 14 and 13 of the address matter
        else {
            // Once a 1 is shifted into the last position, the SR is full.
            if((ShiftRegister & 0x1) == 0x1) {
                ShiftRegister = ShiftRegister >> 1;
                uint8 bit = LoadRegister & 0x1;
                if(bit == 0x1) {
                    ShiftRegister = FNesCPU::SetBit(5,ShiftRegister);
                }
                // only bits 14 and 13 of the address matter
                uint8 registerSelector = (Address >> 13) & 0x3;
                switch(registerSelector) {
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
                ShiftRegister = 0x10; // SR gets cleared
            }
            // Write LSB from LR into SR. The Registers inside MMC1 are 5-bit wide. 
            else {
                ShiftRegister = ShiftRegister >> 1;
                uint8 bit = LoadRegister & 0x1;
                if(bit == 0x1) {
                    ShiftRegister = FNesCPU::SetBit(5,ShiftRegister);
                }
            }
        }
    }
}