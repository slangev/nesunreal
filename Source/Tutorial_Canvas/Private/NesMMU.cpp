// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMMU.h"

NesMMU::NesMMU()
{
    m_cpuRam = make_unique<vector<uint8>>(m_memorySize,0);
}

NesMMU::~NesMMU()
{
}

void NesMMU::AttachCart(unique_ptr<NesCart> cart) {
    this->m_cart = std::move(cart);
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
    }
    /*if (address >= 0x0000 && address <= 0x1FFF) {
        cpuRam[address & 0x7FF] = data;
    }*/
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
    /*if (address >= 0x0000 && address <= 0x1FFF){
        return cpuRam[address & 0x7FF];
    }*/ 
    // Cart Ram
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        return m_cart->Read(Address - 0x6000);
    // Cart Rom
    } else if (Address >= 0x8000 && Address <= 0x10000){
        return m_cart->Read(Address - 0x8000);
    }
    return 0xFF;
}
