// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMC1.h"

NesMMC1::NesMMC1()
{
}

NesMMC1::NesMMC1(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PRGRomMemory = PRGRomMemory;
    this->PRGRamMemory = PRGRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
}


NesMMC1::~NesMMC1()
{
}

uint8 NesMMC1::Read(unsigned short Address) {
    uint8 returnData = 0xFF;

    // CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
    if (Address >= 0x6000 && Address <= 0x7FFF) {
        returnData = PRGRamMemory->at(Address - 0x6000);
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
        
    } 
    // PPU $0000-$0FFF: 4 KB switchable CHR bank
    else if (Address >= 0x0000 && Address <= 0x0FFF) {
        
    }
    // PPU $1000-$1FFF: 4 KB switchable CHR bank
    else if (Address >= 0x1000 && Address <= 0x1FFF) {
        
    }
    // CPU $8000-$FFFF is connected to a common shift register
    if(Address >= 0x8000 && Address <= 0xFFFF) {
        // CPU $8000-$BFFF: 16 KB PRG ROM bank, either switchable or fixed to the first bank
        if (Address >= 0x8000 && Address <= 0xBFFF) {
            
        }
        // CPU $C000-$FFFF: 16 KB PRG ROM bank, either fixed to the last bank or switchable 
        else if (Address >= 0xC000 && Address <= 0xFFFF) {
        
        }
    }
}