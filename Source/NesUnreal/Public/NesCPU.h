// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesMMU.h"
#include "NesPRegister.h"
#include <memory>
#include <sstream>
#include <iomanip>      // std::setfill, std::setw


/**
 * 
 */
class NESUNREAL_API FNesCPU
{
public:
	FNesCPU();
    explicit FNesCPU(bool bTesting);
	~FNesCPU();
	void AttachMemory(shared_ptr<NesMMU> Mmu);
	void HandleInterrupts();
	void Reset();
    uint Tick();
    unique_ptr<FNesPRegister> P;

    unsigned short PC;
    uint8 SP;
    uint8 A;
    uint8 X;
    uint8 Y;
	uint LastCycleCount = 0;
	bool bTesting = false;
    
private:
	shared_ptr<NesMMU> M_Mmu;
    uint TotalCycles = 0;
    uint LineNumber = 1;
	bool bReset = false;

    static const uint CycleCount[];
    void PrintNesTestLogLine(uint8 Opcode);
    uint HandleInstructions(uint8 Opcode);
	static void SeparateWordToBytes(ushort Word, uint8 Result[]);
	static unsigned short CombineBytePairIntoUShort(uint8 Lsb, uint8 Msb);
    unsigned short GetIndirectAddress(const uint8 Reg);
	unsigned short GetIndirectIndexed(const uint8 Reg, bool bCanCross);
	uint8 GetAbsoluteRead(const uint8 Reg, bool bCanCross);
	unsigned short GetAbsoluteAddress(const uint8 Reg, bool bCanCross);
	void Jmp(uint8 Opcode);
    void Cp(uint8 Opcode, uint8 Reg);
	uint8 Ld(const uint8 Opcode);
    uint8 Inc(uint8 Opcode, uint8 Reg);
    uint8 Dec(uint8 Opcode, uint8 Reg);
    void Store(uint8 Opcode, uint8 Reg);
    void Rts(uint8 Opcode);
    void Rti(uint8 Opcode);
    void Plp(uint8 Opcode);
    void Pha(uint8 Opcode);
    void And(uint8 Opcode);
    void Adc(uint8 Opcode);
    void Ora(uint8 Opcode);
    void Php(uint8 Opcode);
    void Pla(uint8 Opcode);
    void Cmp(uint8 Opcode);
    void Sbc(uint8 Opcode);
	void Lax(uint8 Opcode);
	void Sax(uint8 Opcode);
	void Dcp(uint8 Opcode);
	void Isb(uint8 Opcode);
	void Slo(uint8 Opcode);
	void Rla(uint8 Opcode);
	void Sre(uint8 Opcode);
	void Rra(uint8 Opcode);
	void Brk(uint8 Opcode);
	void Las(uint8 Opcode);
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
    void Eor(uint8 Opcode);
    uint8 Rol(uint8 Opcode, uint8 Reg) const;
    uint8 Lsr(uint8 Opcode, uint8 Data) const;
    uint8 Asl(uint8 Opcode, uint8 Reg) const;
    uint8 Ror(uint8 Opcode, uint8 Reg) const;
	static uint8 GetBit(uint8 Pos, uint8 Reg);
	static uint8 SetBit(uint8 Pos, uint8 Reg);
	static uint8 ResetBit(uint8 Pos, uint8 Reg);
};
