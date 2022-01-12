// Fill out your copyright notice in the Description page of Project Settings.


#include "NesCPUMMU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesCPUMMU,Log,All)

NesCPUMMU::NesCPUMMU()
{
    m_cpuRam = make_unique<vector<uint8>>(m_memoryRamSize,0);
}

NesCPUMMU::~NesCPUMMU()
{
}

void NesCPUMMU::AttachCart(shared_ptr<NesCart> cart) {
    this->m_cart = cart;
}

void NesCPUMMU::AttachPPU(shared_ptr<NesPPU> ppu) {
    this->m_ppu = ppu;
}

void NesCPUMMU::AttachController(shared_ptr<NesController> controller) {
    this->m_controller = controller;
}

bool NesCPUMMU::RequestNMIInterrupt() {
    return m_ppu->GetNMIInterrupt();
}

void NesCPUMMU::Write(const unsigned short Address, const uint8 Data)
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
        // with mirror handling
        m_ppu->WriteRegister(Address & 0x2007, Data);
    } 
    else if (Address >= 0x4000 && Address <= 0x401F) {
            // APU and I/O Registers
            // Implement as needed

            // APU
            // if (Address <= 0x4013 || Address == 0x4015 || Address == 0x4017){
            //     //APU.writeData(address, data);
            // }

            // OAM DMA
            if (Address == 0x4014){
                unsigned short OAMDMAaddress = (Data << 8);
                for (int i = 0x00; i < 0xFF; i++){
                   m_ppu->ProcessDMA(i, Read(OAMDMAaddress + i));
                }
                bOAMDMA = true;
            }

            // Controller
            // Only 4016 is used for the controller on write, 4017 writes to an APU register.
            else if (Address == 0x4016){
                m_controller->Write(Address,Data);
            }
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

uint8 NesCPUMMU::Read(const unsigned short Address) {
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
        return m_ppu->ReadRegister(Address & 0x2007);
    }
    // Controllers
    else if (Address >= 0x4016 && Address <= 0x4017) {
        return m_controller->Read(0x4016);
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
