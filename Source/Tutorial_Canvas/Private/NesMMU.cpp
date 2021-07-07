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

void NesMMU::Write(unsigned short address, uint8 data) {
    //CPU RAM
    if(address >= 0x0000 && address < 0x2000) {
        //Zero Page
        if(address >= 0x0000 && address < 0x0100) {
            m_cpuRam->at(address) = data;
        //Stack
        } else if(address >= 0x0100 && address < 0x0200) {
            m_cpuRam->at(address) = data;
        //Ram
        } else if(address >= 0x200 && address < 0x0800) {
            m_cpuRam->at(address) = data;
        //Mirrors 
        } else {
            m_cpuRam->at(address & 0x7FF) = data;
        }
    }
    /*if (address >= 0x0000 && address <= 0x1FFF) {
        cpuRam[address & 0x7FF] = data;
    }*/
}

uint8 NesMMU::Read(unsigned short address) {
    //CPU RAM
    if(address >= 0x0000 && address < 0x2000) {
        //Zero Page
        if(address >= 0x0000 && address < 0x0100) {
            return m_cpuRam->at(address);
        //Stack
        } else if(address >= 0x0100 && address < 0x0200) {
            return m_cpuRam->at(address);
        //Ram
        } else if(address >= 0x200 && address < 0x0800) {
            return m_cpuRam->at(address);
        //Mirrors 
        } else {
            return m_cpuRam->at(address & 0x7FF);
        }
    }
    /*if (address >= 0x0000 && address <= 0x1FFF){
        return cpuRam[address & 0x7FF];
    }*/ 
    // Cart Ram
    else if (address >= 0x6000 && address <= 0x7FFF){
        return m_cart->Read(address - 0x6000);
    // Cart Rom
    } else if (address >= 0x8000 && address <= 0x10000){
        return m_cart->Read(address - 0x8000);
    }
    return 0xFF;
}
