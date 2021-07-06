// Fill out your copyright notice in the Description page of Project Settings.


#include "NesCPU.h"

DEFINE_LOG_CATEGORY(LogNesCPU);


const int NesCPU::cycleCount[] = {
    7,6,0,8,3,3,5,5,3,2,2,2,4,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
    6,6,0,8,3,3,5,5,4,2,2,2,4,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
    6,6,0,8,3,3,5,5,3,2,2,2,3,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
    6,6,0,8,3,3,5,5,4,2,2,2,5,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
    2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
    2,6,0,6,4,4,4,4,2,5,2,5,5,5,5,5,
    2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
    2,5,0,5,4,4,4,4,2,4,2,4,4,4,4,4,
    2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
    2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
    2,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7,
};

NesCPU::NesCPU()
{
    P = make_unique<NesPRegister>();
    A = 0;
    X = 0;
    Y = 0;
    SP = 0xFD;
    totalCycles = 7;
}

NesCPU::~NesCPU()
{
}

void NesCPU::printNesTestLogLine(uint8 opcode) {
    string result = to_string(lineNumber++) + " ";
    stringstream streamPC;
    stringstream streamOPcode;
    stringstream streamA;
    stringstream streamX;
    stringstream streamY;
    stringstream streamP;
    stringstream streamSP;
    streamPC << setfill ('0') << setw(sizeof(unsigned short)*2) << hex << PC;
    streamOPcode << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)opcode;
    streamA << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)A;
    streamX << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)X;
    streamY << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)Y;
    streamP << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)P->pState();
    streamSP << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)SP;

    result += streamPC.str() + "  ";
    result += streamOPcode.str() + "                       A:";
    result += streamA.str() + " X:";
    result += streamX.str() + " Y:";
    result += streamY.str() + " P:";
    result += streamP.str() + " SP:";
    result += streamSP.str() + " CYC:";
    result += to_string(totalCycles);

    FString ueresult(result.c_str());
    ueresult = ueresult.ToUpper();
    UE_LOG(LogNesCPU, Log, TEXT("%s"), *ueresult);
}

uint8 NesCPU::Tick() {
    return 4;
}

void NesCPU::AttachMemory(shared_ptr<NesMMU> mmu, unsigned short startPC) {
    this->m_mmu = mmu;
    this->PC = (startPC == 0) ? ((m_mmu->Read(0xFFFD) << 8) | m_mmu->Read(0xFFFC)) : startPC;
    printNesTestLogLine(0x25);
}
