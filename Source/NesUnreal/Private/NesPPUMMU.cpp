// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPPUMMU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesPPUMMU,Log,All)


NesPPUMMU::NesPPUMMU(shared_ptr<NesCart> cart)
{
    oam = make_unique<vector<int>>(0x100); // Spr-RAM The PPU also has a separate 256 byte area of memory, SPR-RAM (Sprite RAM), to store the sprite attributes. The sprites themselves can be found in the pattern tables
    paletteRAM = make_unique<vector<int>>(0x20); // 3F20-3F00 = 0x20
    nameSpaceTable = make_unique<vector<int>>(0x800); // VRAM only contains 2KB onboard the console, but Nametable region contains 4kb region. The NES only has 2 KB to store name tables and attribute tables, allowing it to store two of each.
    this->cart = cart;
}

NesPPUMMU::~NesPPUMMU()
{
}

uint8 NesPPUMMU::Read(unsigned short Address) const {

    //Read from pattern table/chrrom/chrram
    if(Address >= 0x0000 && Address < 0x2000) {
        return cart->Read(Address);
    } else if (Address >= 0x2000 && Address < 0x3F00) {
        if(Address >= 0x3000 && Address < 0x3F00) {
            return Read(Address - 0x1000);
        }
        int VRAMAddress = 0;
        int mirrorMode = cart->GetMirrorMode();
        switch (mirrorMode){
            case 0:
                // Single screen, lower bank
                VRAMAddress = Address & 0x3FF;
                break;
            case 1:
                // Single screen, upper bank
                VRAMAddress = (Address & 0x3FF) | 0x400;
                break;
            case 2:
                // Vertical
                VRAMAddress = Address & 0x7FF;
                break;
            case 3:
                // Horizontal
                VRAMAddress = Address & 0x3FF;
                if ((Address & 0x800) > 0){
                    VRAMAddress |= 0x400;
                }
                break;
            case 0xFF:
                VRAMAddress = 0;
                // Vertical mirroring
                if (cart->Header->Mirroring == 1){
                    VRAMAddress = Address & 0x7FF;
                }
                // Horizontal mirroring
                else{
                    VRAMAddress = Address & 0x3FF;
                    if (cart->Header->Mirroring == 0){
                        VRAMAddress |= 0x400;
                    }
                }
                break;
        }

        return nameSpaceTable->at(VRAMAddress);
    } 
    // Palette Ram
    else if(Address >= 0x3F00 && Address < 0x4000) {

        if(Address >= 0x3F20 && Address < 0x4000) {
            return Read(Address & 0x3F1F);
        }
        // Palette Mirroring.
        if ((Address & 0x3) == 0){
            Address &= 0xFF0F;
        }
        return paletteRAM->at(Address & 0x1F); // 0x1F is (paletteRam->size() -1)
    }
    //Mirrors of 0x0000-0x3FFF 
    else if(Address >= 0x4000 && Address < 0x10000) {
        return Read(Address & 0x3FFF);
    }
    return 0xFF;
}

void NesPPUMMU::Write(unsigned short Address, uint8 Data) const {
    if(Address >= 0x0000 && Address < 0x2000) {
        cart->Write(Address,Data);
    } else if (Address >= 0x2000 && Address < 0x3F00) {
        if(Address >= 0x3000 && Address < 0x3F00) {
            Write(Address - 0x1000, Data);
            return;
        }
        int VRAMAddress = 0;
        int mirrorMode = cart->GetMirrorMode();
        switch (mirrorMode){
            case 0:
                // Single screen, lower bank
                VRAMAddress = Address & 0x3FF;
                break;
            case 1:
                // Single screen, upper bank
                VRAMAddress = (Address & 0x3FF) | 0x400;
                break;
            case 2:
                // Vertical
                VRAMAddress = Address & 0x7FF;
                break;
            case 3:
                // Horizontal
                VRAMAddress = Address & 0x3FF;
                if ((Address & 0x800) > 0){
                    VRAMAddress |= 0x400;
                }
                break;
            case 0xFF:
                VRAMAddress = 0;
                // Vertical mirroring
                if (cart->Header->Mirroring == 1){
                    VRAMAddress = Address & 0x7FF;
                }
                // Horizontal mirroring
                else{
                    VRAMAddress = Address & 0x3FF;
                    if (cart->Header->Mirroring == 0){
                        VRAMAddress |= 0x400;
                    }
                }
                break;
        }

        nameSpaceTable->at(VRAMAddress) = Data;
    } 
    // Palette Ram
    else if(Address >= 0x3F00 && Address < 0x4000) {

        if(Address >= 0x3F20 && Address < 0x4000) {
            Write((Address & 0x3F1F), Data);
            return;
        }
        // Palette Mirroring.
        if ((Address & 0x3) == 0){
            Address &= 0xFF0F;
        }
        paletteRAM->at(Address & 0x1F) = Data; // 0x1F is (paletteRam->size() -1)
    }
    //Mirrors of 0x0000-0x3FFF 
    else if(Address >= 0x4000 && Address < 0x10000) {
        Write((Address & 0x3FFF), Data);
    }
}

