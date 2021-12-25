// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPPU_MMU.h"

NesPPU_MMU::NesPPU_MMU()
{
    oam = make_unique<vector<int>>();
    paletteRAM = make_unique<vector<int>>();
    nameSpaceTable = make_unique<vector<int>>();
}

NesPPU_MMU::~NesPPU_MMU()
{
}
