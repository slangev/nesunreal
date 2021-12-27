// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesCPUMMU,Log,All)

NesMMU::NesMMU()
{
    m_cpuRam = make_unique<vector<uint8>>(m_memoryRamSize,0);
}

NesMMU::~NesMMU()
{
}

void NesMMU::AttachCart(shared_ptr<NesCart> cart) {
    this->m_cart = cart;
}

void NesMMU::Write(const unsigned short Address, const uint8 Data) const
{
    //CPU RAM
    if(Address >= 0x0000 && Address < 0x2000) {
        //Zero Page
        if(Address >= 0x0000 && Address < 0x0100) {
            m_cpuRam->at(Address) = Data;
        //Stack
        } else if(Address >= 0x0100 && Address < 0x0200) {
            m_cpuRam->at(Address) = Data;
        //Ram
        } else if(Address >= 0x200 && Address < 0x0800) {
            m_cpuRam->at(Address) = Data;
        //Mirrors 
        } else {
            m_cpuRam->at(Address & 0x7FF) = Data;
        }
    } // PPU Registers
    else if (Address >= 0x2000 && Address <= 0x3FFF) {
        // with mirrors handling
        //m_ppu->at(Address & 0x2007);
    }
    else if (Address >= 0x4020 && Address <= 0x5FFF) {
        // Cartridge expansion rom, not implemented
        UE_LOG(LogNesCPUMMU, Log, TEXT("Write Cartridge expansion rom, not implemented"));
    }
    // Cart SRam
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        m_cart->Write(Address, Data);
    // Cart Rom
    } else if (Address >= 0x8000 && Address <= 0xFFFF){
        UE_LOG(LogNesCPUMMU, Log, TEXT("Trying to write outside SRAM space."));
        m_cart->Write(Address, Data);
    }
}

uint8 NesMMU::Read(const unsigned short Address) const {
    //CPU RAM
    if(Address >= 0x0000 && Address < 0x2000) {
        //Zero Page
        if(Address >= 0x0000 && Address < 0x0100) {
            return m_cpuRam->at(Address);
        //Stack
        } else if(Address >= 0x0100 && Address < 0x0200) {
            return m_cpuRam->at(Address);
        //Ram
        } else if(Address >= 0x200 && Address < 0x0800) {
            return m_cpuRam->at(Address);
        //Mirrors 
        } else {
            return m_cpuRam->at(Address & 0x7FF);
        }
    } 
    // PPU Registers
    else if (Address >= 0x2000 && Address <= 0x3FFF) {
        // with mirrors handling
        //m_ppu->at(Address & 0x2007);
    }
    else if (Address >= 0x4020 && Address <= 0x5FFF) {
        // Cartridge expansion rom, not implemented
        UE_LOG(LogNesCPUMMU, Log, TEXT("Read Cartridge expansion rom, not implemented"));
    }
    // Cart SRam
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        return m_cart->Read(Address);
    // Cart Rom
    } else if (Address >= 0x8000 && Address <= 0xFFFF){
        return m_cart->Read(Address);
    }
    return 0xFF;
}
