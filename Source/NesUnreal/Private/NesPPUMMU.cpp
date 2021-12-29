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
