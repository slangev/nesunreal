// Fill out your copyright notice in the Description page of Project Settings.


#include "NesCPUMMU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesCPUMMU,Log,All)

NesCPUMMU::NesCPUMMU()
{
    M_CPURam = make_unique<vector<uint8>>(M_MemoryRamSize,0);
}

NesCPUMMU::~NesCPUMMU()
{
}

void NesCPUMMU::AttachCart(shared_ptr<NesCart> Cart) {
    this->M_Cart = Cart;
}

void NesCPUMMU::AttachPpu(shared_ptr<NesPPU> Ppu) {
    this->M_Ppu = Ppu;
}

void NesCPUMMU::AttachController(shared_ptr<NesController> Controller) {
    this->M_Controller = Controller;
}

void NesCPUMMU::AttachApu(UNesApu *Apu)
{
    this->M_Sound = Apu;
}

bool NesCPUMMU::RequestNmiInterrupt() const
{
    return M_Ppu->GetNMIInterrupt();
}

bool NesCPUMMU::RequestIrqInterrupt() const
{
    return M_Cart->GetIRQRequested();
}

bool NesCPUMMU::RequestFrameInterrupt() const
{
    return M_Sound->FrameInterrupt();
}

void NesCPUMMU::Write(const unsigned short Address, const uint8 Data)
{
    //CPU RAM
    if(Address >= 0x0000 && Address < 0x2000) {
        //Zero Page
        if(Address >= 0x0000 && Address < 0x0100) {
            M_CPURam->at(Address) = Data;
        //Stack
        } else if(Address >= 0x0100 && Address < 0x0200) {
            M_CPURam->at(Address) = Data;
        //Ram
        } else if(Address >= 0x200 && Address < 0x0800) {
            M_CPURam->at(Address) = Data;
        //Mirrors 
        } else {
            M_CPURam->at(Address & 0x7FF) = Data;
        }
    } // PPU Registers
    else if (Address >= 0x2000 && Address <= 0x3FFF) {
        // with mirror handling
        M_Ppu->WriteRegister(Address & 0x2007, Data);
    } 
    else if (Address >= 0x4000 && Address <= 0x401F) {
            // APU and I/O Registers
            // Implement as needed

            // APU
            if (Address <= 0x4013 || Address == 0x4015 || Address == 0x4017){
                M_Sound->Write(Address, Data);
            }

            // OAM DMA
            if (Address == 0x4014){
                const unsigned short OAMDMAaddress = (Data << 8);
                for (int i = 0x00; i < 0x100; i++){
                   M_Ppu->ProcessDMA(i, Read(OAMDMAaddress + i));
                }
                bOAMDMA = true;
            }

            // Controller
            // Only 4016 is used for the controller on write, 4017 writes to an APU register.
            else if (Address == 0x4016){
                M_Controller->Write(Address,Data);
            }
        }
    
    else if (Address >= 0x4020 && Address <= 0x5FFF) 
    {
        // Cartridge expansion rom, not implemented
        UE_LOG(LogNesCPUMMU, Warning, TEXT("Write Cartridge expansion rom, not implemented"));
        M_Cart->Write(Address, Data);
    }
    // Cart SRam
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        M_Cart->Write(Address, Data);
    // Cart Rom
    } else if (Address >= 0x8000 && Address <= 0xFFFF){
        M_Cart->Write(Address, Data);
    }
}

uint8 NesCPUMMU::Read(const unsigned short Address) {
    //CPU RAM
    if(Address >= 0x0000 && Address < 0x2000) {
        //Zero Page
        if(Address >= 0x0000 && Address < 0x0100) {
            return M_CPURam->at(Address);
        //Stack
        } else if(Address >= 0x0100 && Address < 0x0200) {
            return M_CPURam->at(Address);
        //Ram
        } else if(Address >= 0x200 && Address < 0x0800) {
            return M_CPURam->at(Address);
        //Mirrors 
        } else {
            return M_CPURam->at(Address & 0x7FF);
        }
    } 
    // PPU Registers
    else if (Address >= 0x2000 && Address <= 0x3FFF) {
        // with mirrors handling
        return M_Ppu->ReadRegister(Address & 0x2007);
    }
    else if (Address == 0x4015)
    {
        return M_Sound->Read(Address);
    }
    // Controllers
    else if (Address >= 0x4016 && Address <= 0x4017) {
        return M_Controller->Read(Address);
    }
    else if (Address >= 0x4020 && Address <= 0x5FFF) {
        // Cartridge expansion rom, not implemented
        UE_LOG(LogNesCPUMMU, Log, TEXT("Read Cartridge expansion rom, not implemented"));
        return M_Cart->Read(Address);
    }
    // Cart SRam
    else if (Address >= 0x6000 && Address <= 0x7FFF){
        return M_Cart->Read(Address);
    // Cart Rom
    } else if (Address >= 0x8000 && Address <= 0xFFFF){
        return M_Cart->Read(Address);
    }
    return 0xFF;
}
