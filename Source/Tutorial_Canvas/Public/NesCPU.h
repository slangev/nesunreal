// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesMMU.h"
#include "NesPRegister.h"
#include <memory>

/**
 * 
 */
class TUTORIAL_CANVAS_API NesCPU
{
public:
	NesCPU();
	~NesCPU();
	void AttachMemory(shared_ptr<NesMMU> mmu);
    uint8 Tick();
	
private:
	shared_ptr<NesMMU> m_mmu;
    unique_ptr<NesPRegister> P;
    ushort PC;
    uint8 SP;
    uint8 A;
    uint8 X;
    uint8 Y;
    uint totalCycles = 0;
    uint lineNumber = 1;

    static const int cycleCount[];
};
