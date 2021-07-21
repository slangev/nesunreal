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
    unique_ptr<NesPRegister> P;
    unsigned short PC;
    uint8 SP;
    uint8 A;
    uint8 X;
    uint8 Y;
    
private:
	shared_ptr<NesMMU> m_mmu;
    uint totalCycles = 0;
    uint lineNumber = 1;

    static const uint cycleCount[];
    void PrintNesTestLogLine(uint8 Opcode);
    uint HandleInstructions(uint8 Opcode);
	static void SeparateWordToBytes(ushort Word, uint8 Result[]);
	static unsigned short CombineBytePairIntoUShort(uint8 lsb, uint8 msb);
    unsigned short GetIndirectAddress(uint8 Reg);
    void Jmp(uint8 Opcode);
    void Cp(uint8 Opcode, uint8 Reg);
    uint8 Ld(uint8 Opcode);
    uint8 Inc(uint8 Opcode, uint8 Reg) const;
    uint8 Dec(uint8 Opcode, uint8 Reg) const;
    void Store(uint8 Opcode, uint8 Reg);
    void Rts(uint8 Opcode);
    void Rti(uint8 Opcode);
    void Plp(uint8 Opcode);
    void Pha(uint8 Opcode);
    void And(uint8 Opcode);
    void Adc(uint8 Opcode);
    void Ora(uint8 opcode);
    void Php(uint8 Opcode);
    void Pla(uint8 Opcode);
    void Cmp(uint8 Opcode);
    void Sbc(uint8 Opcode);
	static void Nop(uint8 Opcode);
    void Jsr(uint8 Opcode);
    void Bit(uint8 Opcode);
    uint Branch(uint8 Opcode, bool bCC);
    uint8 Transfer(uint8 Opcode, uint8 Reg) const;
    void Sec(uint8 Opcode) const;
    void Sei(uint8 Opcode) const;
    void Sed(uint8 Opcode) const;
    void Clc(uint8 Opcode) const;
    void Cld(uint8 Opcode) const;
    void Clv(uint8 Opcode) const;
    void Eor(uint8 opcode);
    uint8 Rol(uint8 Opcode, uint8 Reg) const;
    uint8 Lsr(uint8 Opcode, uint8 Data) const;
    uint8 Asl(uint8 Opcode, uint8 Reg) const;
    uint8 Ror(uint8 Opcode, uint8 Reg) const;
	static uint8 GetBit(uint8 Pos, uint8 Reg);
	static uint8 SetBit(uint8 Pos, uint8 Reg);
	static uint8 ResetBit(uint8 Pos, uint8 Reg);
};
