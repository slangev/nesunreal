// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesMMU.h"
#include "NesPRegister.h"
#include <memory>
#include <iomanip>
#include <sstream>
DECLARE_LOG_CATEGORY_EXTERN(LogNesCPU, Log, All);

/**
 * 
 */
class TUTORIAL_CANVAS_API NesCPU
{
public:
	NesCPU();
	~NesCPU();
	void AttachMemory(shared_ptr<NesMMU> mmu, unsigned short PC);
    uint Tick();
    
	
private:
	shared_ptr<NesMMU> m_mmu;
    unique_ptr<NesPRegister> P;
    unsigned short PC;
    uint8 SP;
    uint8 A;
    uint8 X;
    uint8 Y;
    uint totalCycles = 0;
    uint lineNumber = 1;

    static const uint cycleCount[];
    void printNesTestLogLine(uint8 opcode);
    uint handleInstructions(uint8 opcode);
    void separateWordToBytes(ushort word, uint8 result[]);
    unsigned short combineBytePairIntoUShort(uint8 lsb, uint8 msb);
    unsigned short getIndirectAddress(uint8 reg);
    void JMP(uint8 opcode);
    void CP(uint8 opcode, uint8 reg);
    uint8 LD(uint8 opcode);
    void STORE(uint8 opcode, uint8 reg);
    void RTS(uint8 opcode);
    void RTI(uint8 opcode);
    void PLP(uint8 opcode);
    void PHA(uint8 opcode);
    void AND(uint8 opcode);
    void ADC(uint8 opcode);
    void ORA(uint8 opcode);
    void PHP(uint8 opcode);
    uint8 ROL(uint8 opcode, uint8 reg);
    uint8 LSR(uint8 opcode, uint8 reg);
    uint8 ASL(uint8 opcode, uint8 reg);
    uint8 ROR(uint8 opcode, uint8 reg);
    uint8 getBit(uint8 pos, uint8 reg);
    uint8 setBit(uint8 pos, uint8 reg);
    uint8 resetBit(uint8 pos, uint8 reg);
};
