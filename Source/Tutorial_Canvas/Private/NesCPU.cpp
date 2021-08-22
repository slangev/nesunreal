// Fill out your copyright notice in the Description page of Project Settings.

//https://www.nesdev.com/undocumented_opcodes.txt (Illegal Opcodes)
#include "NesCPU.h"

DEFINE_LOG_CATEGORY(LogNesCPU);


constexpr uint NesCPU::CycleCount[] = { //157
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
    PC = 0x8000;
    A = 0;
    X = 0;
    Y = 0;
    SP = 0xFD;
    TotalCycles = 7;
}

NesCPU::~NesCPU()
{
}

void NesCPU::PrintNesTestLogLine(uint8 Opcode) {
    string Result = to_string(LineNumber++) + " ";
    stringstream StreamPC;
    stringstream StreamOpCode;
    stringstream StreamA;
    stringstream StreamX;
    stringstream StreamY;
    stringstream StreamP;
    stringstream StreamSP;
    StreamPC << setfill ('0') << setw(sizeof(unsigned short)*2) << hex << PC-1;
    StreamOpCode << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(Opcode);
    StreamA << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(A);
    StreamX << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(X);
    StreamY << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(Y);
    StreamP << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(P->pState());
    StreamSP << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(SP);

    Result += StreamPC.str() + "  ";
    Result += StreamOpCode.str() + "                       A:";
    Result += StreamA.str() + " X:";
    Result += StreamX.str() + " Y:";
    Result += StreamY.str() + " P:";
    Result += StreamP.str() + " SP:";
    Result += StreamSP.str() + " CYC:";
    Result += to_string(TotalCycles);

    FString DebugResult(Result.c_str());
    DebugResult = DebugResult.ToUpper();
    UE_LOG(LogNesCPU, Log, TEXT("%s"), *DebugResult);
}

void LogOpcode(FString msg, uint8 opcode) {
    string Result = "";
    stringstream StreamOpCode;
    StreamOpCode << setfill ('0') << setw(sizeof(uint8)*2) << hex << static_cast<uint>(opcode);
    Result += StreamOpCode.str();
    FString DebugResult(Result.c_str());
    DebugResult = DebugResult.ToUpper();
    UE_LOG(LogNesCPU, Log, TEXT("%s %s"),*msg, *DebugResult);
}

uint NesCPU::Tick() {
    uint8 const Opcode = m_mmu->Read(PC++ & 0xFFFF);
    PrintNesTestLogLine(Opcode);
    return HandleInstructions(Opcode);
}


uint NesCPU::HandleInstructions(const uint8 Opcode) {
        LastCycleCount = CycleCount[Opcode];
        switch(Opcode) {
            case 0x01:
                Ora(Opcode);
                break;
            case 0x03:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x04:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x05:
                Ora(Opcode);
                break;
            case 0x06:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Asl(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x07:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x08:
                Php(Opcode);
                break;
            case 0x09:
                Ora(Opcode);
                break;
            case 0x0A:
                A = Asl(Opcode,A);
                break;
            case 0x0C:
                {
                    //Absolute Noop (Triple NOOP)
                    Nop(Opcode);
                    PC++;
                    PC++;
                    break;
                }
            case 0x0D:
                Ora(Opcode);
                break;
            case 0x0E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Asl(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x0F:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x10:
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->NFlag) == 0);
                break;
            case 0x11:
                Ora(Opcode);
                break;
            case 0x13:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x14:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x15:
                Ora(Opcode);
                break;
            case 0x16:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Asl(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x17:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x18:
                Clc(Opcode);
                break;
            case 0x19:
                Ora(Opcode);
                break;
            case 0x1A:
                {
                    Nop(Opcode);
                    break;
                }
            case 0x1B:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x1C:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0x1D:
                Ora(Opcode);
                break;
            case 0x1E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Asl(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x1F:
                {
                    Slo(Opcode);
                    break;
                }
            case 0x20:
                Jsr(Opcode);
                break;
            case 0x21:
                And(Opcode);
                break;
            case 0x23:
                {
                    Rla(Opcode);
                    break;
                }
            case 0x24:
                Bit(Opcode);
                break;
            case 0x25:
                And(Opcode);
                break;
            case 0x26:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Rol(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x28:
                Plp(Opcode);
                break;
            case 0x29:
                And(Opcode);
                break;
            case 0x2A:
                A = Rol(Opcode,A);
                break;
            case 0x2C:
                {
                    Bit(Opcode);
                    break;
                }
            case 0x2D:
                And(Opcode);
                break;
            case 0x2E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Rol(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x30:
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->NFlag) == 1);
                break;
            case 0x31:
                And(Opcode);
                break;
            case 0x34:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x35:
                {
                    And(Opcode);
                    break;
                }
            case 0x36:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Rol(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x38:
                Sec(Opcode);
                break;
            case 0x39:
                And(Opcode);
                break;
            case 0x3A:
                {
                    Nop(Opcode);
                    break;
                }
            case 0x3C:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0x3D:
                {
                    And(Opcode);
                    break;
                }
            case 0x3E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Rol(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x40:
                Rti(Opcode);
                break;
            case 0x41:
                Eor(Opcode);
                break;
            case 0x44:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x45:
                Eor(Opcode);
                break;
            case 0x46:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Lsr(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x48:
                Pha(Opcode);
                break;
            case 0x49:
                Eor(Opcode);
                break;
            case 0x4A:
                A = Lsr(Opcode,A);
                break;
            case 0x4C:
                Jmp(Opcode);
                break;
            case 0x4D:
                Eor(Opcode);
                break;
            case 0x4E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Lsr(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x50:
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->VFlag) == 0);
                break;
            case 0x51:
                Eor(Opcode);
                break;
            case 0x54:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x55:
                {
                    Eor(Opcode);
                    break;
                }
            case 0x56:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Lsr(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x59:
                {
                    Eor(Opcode);
                    break;
                }
            case 0x5A:
                {
                    Nop(Opcode);
                    break;
                }
            case 0x5C:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0x5D:
                {
                    Eor(Opcode);
                    break;
                }
            case 0x5E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Lsr(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x60:
                Rts(Opcode);
                break;
            case 0x61:
                Adc(Opcode);
                break;
            case 0x64:
                {
                    //Zero Page Noop (Double NOOP)
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x65:
                Adc(Opcode);
                break;
            case 0x66:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Ror(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x68:
                Pla(Opcode);
                break;
            case 0x69:
                Adc(Opcode);
                break;
            case 0x6A:
                A = Ror(Opcode,A);
                break;
            case 0x6C:
                Jmp(Opcode);
                break;
            case 0x6D:
                Adc(Opcode);
                break;
            case 0x6E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Ror(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x70:
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->VFlag) == 1);
                break;
            case 0x71:
                Adc(Opcode);
                break;
            case 0x74:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x75:
                {
                    Adc(Opcode);
                    break;
                }
            case 0x76:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Ror(Opcode,m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0x78:
                Sei(Opcode);
                break;
            case 0x79:
                {
                    Adc(Opcode);
                    break;
                }
            case 0x7A:
                {
                    Nop(Opcode);
                    break;
                }
            case 0x7C:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0x7D:
                {
                    Adc(Opcode);
                    break;
                }
            case 0x7E:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Ror(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0x80:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x81:
                Store(Opcode,X);
                break;
            case 0x82:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x83:
                {
                    Sax(Opcode);
                    break;
                }
            case 0x84:
                Store(Opcode,Y);
                break;
            case 0x85:
                Store(Opcode,A);
                break;
            case 0x86:
                Store(Opcode,X);
                break;
            case 0x87:
                {
                    Sax(Opcode);
                    break;
                }
            case 0x88:
                Y = Dec(Opcode,Y);
                break;
            case 0x89:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0x8A:
                A = Transfer(Opcode,X);
                break;
            case 0x8C:
                Store(Opcode,Y);
                break;
            case 0x8D:
                Store(Opcode,A);
                break;
            case 0x8E:
                Store(Opcode,X);
                break;
            case 0x8F:
                {
                    Sax(Opcode);
                    break;
                }
            case 0x90:
                //BCC
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->CFlag) == 0);
                break;
            case 0x91:
                Store(Opcode,A);
                break;
            case 0x94:
                {
                    Store(Opcode,Y);
                    break;
                }
            case 0x95:
                {
                    Store(Opcode,A);
                    break;
                }
            case 0x96:
                {
                    Store(Opcode,X);
                    break;
                }
            case 0x97:
                {
                    Sax(Opcode);
                    break;
                }
            case 0x98:
                A = Transfer(Opcode,Y);
                break;
            case 0x9A:
                SP = Transfer(Opcode,X);
                break;
            case 0x9D:
                {
                    Store(Opcode,A);
                    break;
                }
            case 0x99:
                {
                    Store(Opcode,A);
                    break; 
                }
            case 0xA0:
                Y = Ld(Opcode);
                break;
            case 0xA1:
                A = Ld(Opcode);
                break;
            case 0xA2:
                X = Ld(Opcode);
                break;
            case 0xA3:
                {
                    A = Ld(Opcode);
                    PC--; // Go back once to load A
                    X = Ld(Opcode);
                    break;
                }
            case 0xA4:
                Y = Ld(Opcode);
                break;
            case 0xA5:
                A = Ld(Opcode);
                break;
            case 0xA6:
                X = Ld(Opcode);
                break;
            case 0xA7:
                {
                    A = Ld(Opcode);
                    PC--; // Go back once to load A
                    X = Ld(Opcode);
                    break;
                }
            case 0xA8:
                Y = Transfer(Opcode,A);
                break;
            case 0xA9:
                A = Ld(Opcode);
                break;
            case 0xAA:
                X = Transfer(Opcode,A);
                break;
            case 0xAC:
                {
                    Y = Ld(Opcode);
                    break;
                }
            case 0xAD:
                A = Ld(Opcode);
                break;
            case 0xAE:
                X = Ld(Opcode);
                break;
            case 0xAF:
                {
                    Lax(Opcode);
                    break;
                }
            case 0xB0:
                //BCS
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->CFlag) == 1);
                break;
            case 0xB1:
                {
                    A = Ld(Opcode);
                    break;
                }
            case 0xB3:
                {
                    Lax(Opcode);
                    break;
                }
            case 0xB4:
                {
                    Y = Ld(Opcode);
                    break;
                }
            case 0xB5:
                {
                    A = Ld(Opcode);
                    break;
                }
            case 0xB6:
                {
                    X = Ld(Opcode);
                    break;
                }
            case 0xB7:
                {
                    Lax(Opcode);
                    break;
                }
            case 0xB8:
                Clv(Opcode);
                break;
            case 0xB9:
                {
                    A = Ld(Opcode);
                    break;
                }
            case 0xBA:
                X = Transfer(Opcode,SP);
                break;
            case 0xBC:
                {
                    Y = Ld(Opcode);
                    break;
                }
            case 0xBD:
                {
                    A = Ld(Opcode);
                    break;
                }
            case 0xBE:
                {
                    X = Ld(Opcode);
                    break;
                }
            case 0xBF:
                {
                    Lax(Opcode);
                    break;
                }
            case 0xC0:
                Cp(Opcode,Y);
                break;
            case 0xC1:
                Cmp(Opcode);
                break;
            case 0xC2:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0xC3:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xC4:
                Cp(Opcode,Y);
                break;
            case 0xC5:
                Cmp(Opcode);
                break;
            case 0xC6:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Dec(Opcode, m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0xC7:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xC8:
                Y = Inc(Opcode,Y);
                break;
            case 0xC9:
                Cmp(Opcode);
                break;
            case 0xCA:
                X = Dec(Opcode,X);
                break;
            case 0xCC:
                Cp(Opcode,Y);
                break;
            case 0xCD:
                Cmp(Opcode);
                break;
            case 0xCE:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Dec(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0xCF:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xD0:
                //BNE
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->ZFlag) == 0);
                break;
            case 0xD1:
                Cmp(Opcode);
                break;
            case 0xD3:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xD4:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0xD5:
                {
                    Cmp(Opcode);
                    break;
                }
            case 0xD6:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Dec(Opcode, m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0xD7:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xD8:
                Cld(Opcode);
                break;
            case 0xD9:
                Cmp(Opcode);
                break;
            case 0xDA:
                {
                    Nop(Opcode);
                    break;
                }
            case 0xDB:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xDC:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0xDD:
                {
                    Cmp(Opcode);
                    break;
                }
            case 0xDE:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Dec(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0xDF:
                {
                    Dcp(Opcode);
                    break;
                }
            case 0xE0:
                Cp(Opcode,X);
                break;
            case 0xE1:
                Sbc(Opcode);
                break;
            case 0xE2:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0xE3:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xE4:
                Cp(Opcode,X);
                break;
            case 0xE5:
                Sbc(Opcode);
                break;
            case 0xE6:
                {
                    const unsigned short Address = m_mmu->Read(PC++);
                    m_mmu->Write(Address, Inc(Opcode, m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0xE7:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xE8:
                X = Inc(Opcode,X);
                break;
            case 0xE9:
            case 0xEB:
                {
                    Sbc(Opcode);
                    break; 
                }
            case 0xEA:
                Nop(Opcode);
                break;
            case 0xEC:
                {
                    Cp(Opcode,X);
                    break;
                }
            case 0xED:
                Sbc(Opcode);
                break;
            case 0xEE:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    m_mmu->Write(Address, Inc(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0xEF:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xF0:
                //BEQ
                LastCycleCount += Branch(Opcode,P->ReadFlag(P->ZFlag) == 1);
                break;
            case 0xF1:
                {
                    Sbc(Opcode);
                    break;
                }
            case 0xF3:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xF4:
                {
                    Nop(Opcode);
                    PC++;
                    break;
                }
            case 0xF5:
                {
                    Sbc(Opcode);
                    break;
                }
            case 0xF6:
                {
                    const unsigned short Address = m_mmu->Read(PC++) + X;
                    m_mmu->Write(Address, Inc(Opcode, m_mmu->Read(Address & 0xFF)));
                    break;
                }
            case 0xF7:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xF8:
                Sed(Opcode);
                break;
            case 0xF9:
                {
                    Sbc(Opcode);
                    break;
                }
            case 0xFA:
                {
                    Nop(Opcode);
                    break;
                }
            case 0xFB:
                {
                    Isb(Opcode);
                    break;
                }
            case 0xFC:
                {
                    Nop(Opcode);
                    GetAbsoluteAddress(X,true);
                    break;
                }
            case 0xFD:
                {
                    Sbc(Opcode);
                    break;
                }
            case 0xFE:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                    m_mmu->Write(Address, Inc(Opcode,m_mmu->Read(Address)));
                    break;
                }
            case 0xFF:
                {
                    Isb(Opcode);
                    break;
                }
            default:
                LogOpcode("Unknown opcode: ", Opcode);
                break;
        }
        TotalCycles += LastCycleCount;
        return LastCycleCount; 
}

void NesCPU::AttachMemory(shared_ptr<NesMMU> mmu, unsigned short startPC) {
    this->m_mmu = mmu;
    this->PC = (startPC == 0) ? ((m_mmu->Read(0xFFFD) << 8) | m_mmu->Read(0xFFFC)) : startPC;
}

unsigned short NesCPU::CombineBytePairIntoUShort(const uint8 Lsb, const uint8 Msb) {
    const unsigned short Result = ((Msb << 8) | Lsb);
    return Result;
}

inline uint8 NesCPU::GetAbsoluteRead(const uint8 Reg, bool bCanCross) {
    const unsigned short Address = GetAbsoluteAddress(Reg,bCanCross);
    return m_mmu->Read(Address);
}

inline unsigned short NesCPU::GetAbsoluteAddress(const uint8 Reg, const bool bCanCross)
{
    const uint8 LowerByte = m_mmu->Read(PC++);
    const uint8 UpperByte = m_mmu->Read(PC++);
    unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
    const uint8 PrevPage = static_cast<uint8>((Address & 0xFF00) >> 8);
    Address += Reg;
    // ReSharper disable once CppTooWideScope
    const uint8 CurrPage = static_cast<uint8>((Address & 0xFF00) >> 8);
    if(CurrPage != PrevPage && bCanCross) LastCycleCount++;
    return Address;
}

// Item1 = MSB and Item 2 = LSB
void NesCPU::SeparateWordToBytes(const ushort Word, uint8 Result[]) {
    Result[0] = ((Word & 0xFF00) >> 8);
    Result[1] = (Word & 0x00FF);
}

uint8 NesCPU::GetBit(const uint8 Pos, const uint8 Reg) {
    uint8 Result = static_cast<uint8>((Reg & (1 << Pos)));
    Result = static_cast<uint8>(Result >> Pos); 
    return Result;
}

uint8 NesCPU::SetBit(const uint8 Pos, const uint8 Reg) {
    const uint8 Result = static_cast<uint8>(Reg | (1 << Pos));
    return Result;
}

uint8 NesCPU::ResetBit(const uint8 Pos, const uint8 Reg) {
    const uint8 Result = static_cast<uint8>((Reg & ~(1 << Pos)));
    return Result;
}

// The 6502 only supported Indirect Addressing using X,Y registers
unsigned short NesCPU::GetIndirectAddress(const uint8 Reg) {
    uint8 LowerByte = m_mmu->Read(PC++);
    LowerByte += Reg;
    const uint8 AddressLsb = m_mmu->Read(LowerByte); // Read the lsb of our target address.
    const uint8 AddressMsb = m_mmu->Read(static_cast<uint8>(LowerByte+1)); // Read the msb of our target address.
    const unsigned short Address = CombineBytePairIntoUShort(AddressLsb,AddressMsb);
    return Address;
}

unsigned short NesCPU::GetIndirectIndexed(const uint8 Reg, bool bCanCross)
{
    const uint8 LowerByte = m_mmu->Read(PC++);
    const uint8 AddressLsb = m_mmu->Read(LowerByte); // Read the lsb of our target address.
    const uint8 AddressMsb = m_mmu->Read(static_cast<uint8>(LowerByte+1)); // Read the msb of our target address.
    unsigned short Address = CombineBytePairIntoUShort(AddressLsb,AddressMsb);
    const uint8 PrevPage = static_cast<uint8>((Address & 0xFF00) >> 8);
    Address += static_cast<unsigned short>(Reg);
    // ReSharper disable once CppTooWideScope
    const uint8 CurrPage = static_cast<uint8>((Address & 0xFF00) >> 8);
    if(CurrPage != PrevPage && bCanCross) LastCycleCount++;
    return Address;
}

void NesCPU::Jmp(const uint8 Opcode) {
    //Absolute address mode
    switch(Opcode)
    {
        case 0x4C:
            {
                const uint8 Lsb = m_mmu->Read(PC++);
                const uint8 Msb = m_mmu->Read(PC++);
                PC = CombineBytePairIntoUShort(Lsb,Msb);
                break;
            }
        case 0x6C:
            {
                uint8 Lsb = m_mmu->Read(PC++);
                const uint8 Msb = m_mmu->Read(PC++);
                unsigned short Address = CombineBytePairIntoUShort(Lsb,Msb);
                const uint8 TargetLsb = m_mmu->Read(Address);
                if(Lsb == 0xFF)
                {
                    Lsb = 0x00;
                } else
                {
                    Lsb++;
                }
                Address = CombineBytePairIntoUShort(Lsb,Msb);
                const uint8 TargetMsb = m_mmu->Read(Address);
                PC = CombineBytePairIntoUShort(TargetLsb,TargetMsb);
                break;
            }
    default: ;
    }

}

void NesCPU::Jsr(uint8 Opcode) {
    const uint8 LowByte = m_mmu->Read(PC++);
    const uint8 HighByte = m_mmu->Read(PC++);
    const unsigned short Address = CombineBytePairIntoUShort(LowByte,HighByte);
    uint8 Result[2];
    SeparateWordToBytes(static_cast<unsigned short>(PC - 1), Result);
    m_mmu->Write(SP--|0x100,Result[0]);
    m_mmu->Write(SP--|0x100,Result[1]);
    PC = Address;
}

//The RTS instruction is used at the end of a subroutine to return to the calling routine. It pulls the program counter from the stack.
void NesCPU::Rts(uint8 Opcode) {
    const uint8 LowByte = m_mmu->Read(++SP|0x100);
    const uint8 HighByte = m_mmu->Read(++SP|0x100);
    const unsigned short Address = CombineBytePairIntoUShort(LowByte,HighByte);
    PC = static_cast<unsigned>(Address + 1);
}

void NesCPU::Pha(uint8 Opcode) {
    m_mmu->Write(SP--|0x100,A);
}

void NesCPU::Plp(uint8 Opcode) {
    const uint8 ReadByte = m_mmu->Read(++SP|0x100);
    const uint8 CurrPState = P->pStateWithBFlag();
    P->pSetState(ReadByte);
    (GetBit(5,CurrPState) == 1) ? P->SetFlag(P->XFlag) : P->ResetFlag(P->XFlag);
    (GetBit(4,CurrPState) == 1) ? P->SetFlag(P->BFlag) : P->ResetFlag(P->BFlag);
}

void NesCPU::Rti(const uint8 Opcode) {
    Plp(Opcode); 
    Rts(Opcode); 
    PC -=1;      
}

uint8 NesCPU::Ld(const uint8 Opcode) {
    uint8 ReadByte;
    switch(Opcode) {
        unsigned short Address;
        //Immediate page
        case 0xA0:
        case 0xA2:
        case 0xA9: {
                ReadByte = m_mmu->Read(PC++);
                break;
            }
        //Zero Page
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7: {
                Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        //Zero Page X
        case 0xB4:
        case 0xB5:
            {
                Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        //Zero Page Y
        case 0xB6:
        case 0xB7:
            {
                Address = m_mmu->Read(PC++) + Y;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        //Absolute
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF: {
            const uint8 LowerByte = m_mmu->Read(PC++);
            const uint8 UpperByte = m_mmu->Read(PC++);
            Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        //Indirect,X
        case 0xA1:
        case 0xA3:
            {
                Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        //Indirect,Y
        case 0xB1:
        case 0xB3:
            {
                Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        //Absolute Y
        case 0xB9:
        case 0xBE:
        case 0xBF:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        //Absolute X
        case 0xBC:
        case 0xBD:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
        default:
            //Debug.LogError("Bad LD operation");
            return 0xFF;
    }
    (ReadByte == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,ReadByte) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    return ReadByte;
}

void NesCPU::Cp(const uint8 Opcode, const uint8 Reg) {
    uint8 ReadByte = 0;
    switch(Opcode) {
        case 0xC0:
        case 0xE0:
            {
                ReadByte = m_mmu->Read(PC++); 
                break;
            }
        // Zero page
        case 0xC4:
        case 0xE4:
            {
                const uint8 Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        // Absolute
        case 0xCC:
        case 0xEC:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address);
            }
        default:
            {
                
            }
    }
    (Reg == ReadByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (Reg >= ReadByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(Reg - ReadByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Bit(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    //Zero-page
    if(Opcode == 0x24) {
        const uint8 Address = m_mmu->Read(PC++);
        ReadByte = m_mmu->Read(Address & 0xFF);
    //Absolute
    } else if(Opcode == 0x2C) {
        const uint8 Lsb = m_mmu->Read(PC++);
        const uint8 Msb = m_mmu->Read(PC++);
        const unsigned short Address = CombineBytePairIntoUShort(Lsb,Msb);
        ReadByte = m_mmu->Read(Address);
    }
    // ReSharper disable once CppTooWideScope
    const uint8 Result = static_cast<uint8>(A & ReadByte);
    const uint8 NFlagByte = GetBit(P->NFlag,ReadByte);
    const uint8 VFlagByte = GetBit(P->VFlag,ReadByte);
    (Result == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (NFlagByte == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (VFlagByte == 1) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
}

void NesCPU::Store(const uint8 Opcode, const uint8 Reg) {
    switch(Opcode) {
        // Indirect X
        case 0x81:{
            const unsigned short Address = GetIndirectAddress(X);
            m_mmu->Write(Address,A);
            break;
        } 
        // Zero-page
        case 0x84:
        case 0x85:
        case 0x86: {
            const uint8 Address = m_mmu->Read(PC++);
            m_mmu->Write(Address & 0xFF,Reg);
            break;
        }
        // Absolute
        case 0x8C:
        case 0x8D:
        case 0x8E:{
            const uint8 Lsb = m_mmu->Read(PC++);
            const uint8 Msb = m_mmu->Read(PC++);
            const unsigned short Address = CombineBytePairIntoUShort(Lsb,Msb);
            m_mmu->Write(Address,Reg);
            break;
        }
        case 0x91: {
            const unsigned short Address = GetIndirectIndexed(Y,true);
            m_mmu->Write(Address,Reg);
            break;
        }
        // Zero Page X
        case 0x94:
        case 0x95:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                m_mmu->Write(Address & 0xFF,Reg);
                break;
            }
        case 0x96:
            {
                const uint8 Address = m_mmu->Read(PC++) + Y;
                m_mmu->Write(Address & 0xFF,Reg);
                break;
            }
        case 0x99:
            {
                const unsigned short Address = GetAbsoluteAddress(Y,false);
                m_mmu->Write(Address, Reg);
                break;
            }
        case 0x9D:
            {
                const unsigned short Address = GetAbsoluteAddress(X,false);
                m_mmu->Write(Address, Reg);
                break;
            }
    default: ;
    }
}

void NesCPU::And(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        //Indirect X
        case 0x21:
        {
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        case 0x25:
        {
            const uint8 Address = m_mmu->Read(PC++);
            ReadByte = m_mmu->Read(Address & 0xFF);
            break;
        }
        case 0x29:
        {
            ReadByte = m_mmu->Read(PC++);
            break;
        }
        //Absolute
        case 0x2D:
            {
                const uint8 Lsb = m_mmu->Read(PC++);
                const uint8 Msb = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(Lsb,Msb);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        // Indirect Y
        case 0x31:
            {
                const unsigned Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0x35:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        // Absolute Y
        case 0x39:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        // Absolute X
        case 0x3D:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
    default: ;
    }
    A = static_cast<uint8>(A & ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

uint8 NesCPU::Lsr(const uint8 Opcode, uint8 Data) const {
    const uint8 OldBit = GetBit(0,Data);
    Data = static_cast<uint8>(Data >> 1);
    (Data == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Data) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (OldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return Data;
}

uint8 NesCPU::Asl(uint8 Opcode, uint8 Reg) const {
    const uint8 OldBit = GetBit(7,Reg);
    Reg = static_cast<uint8>(Reg << 1);
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (OldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return Reg;
}

uint8 NesCPU::Ror(uint8 Opcode, uint8 Reg) const {
    const uint8 OldBit = GetBit(0,Reg);
    Reg = static_cast<uint8>(Reg >> 1);
    Reg = (P->ReadFlag(P->CFlag) == 1) ? SetBit(7,Reg) : ResetBit(7,Reg);
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (OldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return Reg;
}

uint8 NesCPU::Rol(uint8 Opcode, uint8 Reg) const {
    const uint8 OldBit = GetBit(7,Reg);
    Reg = static_cast<uint8>(Reg << 1);
    Reg = (P->ReadFlag(P->CFlag) == 1) ? SetBit(0,Reg) : ResetBit(0,Reg);
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (OldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return Reg;
}

void NesCPU::Adc(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        case 0x61:{
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        case 0x65:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0x69:{
            ReadByte = m_mmu->Read(PC++);
            break;
        }
        case 0x6D:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0x71:
            {
                const unsigned Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0x75:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0x79:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        case 0x7D:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
            
    default: ;
    }
    const unsigned short Sum = static_cast<ushort>(A + ReadByte + P->ReadFlag(P->CFlag));
    (static_cast<uint8>(Sum) == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (Sum > 0xFF) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(Sum)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (((A ^ Sum) & (ReadByte ^ Sum) & 0x80) != 0) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    A = static_cast<uint8>(Sum);
}

void NesCPU::Ora(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        //(Indirect,X)
        case 0x01: {
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        //Zero Page
        case 0x05: {
            const unsigned short Address = m_mmu->Read(PC++);
            ReadByte = m_mmu->Read(Address & 0xFF);
            break;
        }
        //Immediate
        case 0x09: {
            ReadByte = m_mmu->Read(PC++);
            break;
        }
        // Absolute
        case 0x0D:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        // Indirect Index Y
        case 0x11:
            {
                const unsigned Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0x15:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        // Absolute Y
        case 0x19:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        // Absolute X
        case 0x1D:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
    default: ;
    }
    A = static_cast<uint8>(A | ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Eor(const uint8 Opcode) {
        uint8 ReadByte = 0x00;
        switch(Opcode) {
            case 0x41:{
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address);
                break;
            }
            case 0x45:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
            case 0x49:
                ReadByte = m_mmu->Read(PC++);
                break;
            case 0x4D:
                {
                    const uint8 LowerByte = m_mmu->Read(PC++);
                    const uint8 UpperByte = m_mmu->Read(PC++);
                    const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                    ReadByte = m_mmu->Read(Address);
                    break;
                }
            // Indirect Y
            case 0x51:
                {
                    const unsigned short Address = GetIndirectIndexed(Y,true);
                    ReadByte = m_mmu->Read(Address);
                    break;
                }
            // ZeroPage X
            case 0x55:
                {
                    const uint8 Address = m_mmu->Read(PC++) + X;
                    ReadByte = m_mmu->Read(Address & 0xFF);
                    break;
                }
            // Absolute Y
            case 0x59:
                {
                    ReadByte = GetAbsoluteRead(Y,true);
                    break;
                }
            case 0x5D:
                {
                    ReadByte = GetAbsoluteRead(X,true);
                    break;
                }
            default: ;
        }
        A = static_cast<uint8>(A ^ ReadByte);
        (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
        (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    }

//https://wiki.nesdev.com/w/index.php/Status_flags
void NesCPU::Php(uint8 Opcode) {
    uint8 CopyP = P->pStateWithBFlag();
    CopyP = static_cast<uint8>(CopyP | 0x30);
    m_mmu->Write(SP--|0x100,CopyP);
}

uint NesCPU::Branch(uint8 Opcode, const bool bCC) {
    const int8 SB = static_cast<int8>(m_mmu->Read(PC++));
    if(bCC) {
        const uint8 PrevPage = static_cast<uint8>((PC & 0xFF00) >> 8);
        PC = static_cast<unsigned short>(PC + SB);
        const uint8 CurrPage = static_cast<uint8>((PC & 0xFF00) >> 8); 
        return static_cast<uint>((CurrPage != PrevPage) ? 2 : 1); // Taken branch and page crossed equals 2. If we take a branch and don't cross pages then return 1.
    }
    return 0;
}

void NesCPU::Sbc(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        case 0xE1:{
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        case 0xE5:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0xE9:
        case 0xEB:
            {
                ReadByte = m_mmu->Read(PC++);
                break;
            }
        case 0xED:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        // Indirect Y
        case 0xF1:
            {
                const unsigned short Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        // ZeroPage X
        case 0xF5:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0xF9:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        case 0xFD:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
        default: ;
    }
    const unsigned short Diff = static_cast<unsigned short>(A - ReadByte - (1 - P->ReadFlag(P->CFlag)));
    (static_cast<uint8>(Diff) == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (Diff <= 0xFF) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(Diff)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (((A ^ Diff) & (~ReadByte ^ Diff) & 0x80) != 0) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    A = static_cast<uint8>(Diff);
}

void NesCPU::Pla(uint8 Opcode) {
    A = m_mmu->Read(++SP|0x100);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Sec(uint8 Opcode) const {
    P->SetFlag(P->CFlag);
}

void NesCPU::Sei(uint8 Opcode) const {
    P->SetFlag(P->IFlag);
}

void NesCPU::Sed(uint8 Opcode) const {
    P->SetFlag(P->DFlag);
}

void NesCPU::Clc(uint8 Opcode) const {
    P->ResetFlag(P->CFlag);
}

void NesCPU::Cld(uint8 Opcode) const {
    P->ResetFlag(P->DFlag);
}

void NesCPU::Clv(uint8 Opcode) const {
    P->ResetFlag(P->VFlag);
}


uint8 NesCPU::Transfer(const uint8 Opcode, const uint8 Reg) const {
    switch(Opcode) {
        case 0x9A: 
            return Reg;
        default:
            (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return Reg;
    }
}

void NesCPU::Cmp(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        case 0xC1: {
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        case 0xC5:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0xC9:
            ReadByte = m_mmu->Read(PC++);
            break;
        //Absolute
        case 0xCD:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0xD1:
            {
                const unsigned short Address = GetIndirectIndexed(Y,true);
                ReadByte = m_mmu->Read(Address);
                break;
            }
        case 0xD5:
            {
                const uint8 Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        case 0xD9:
            {
                ReadByte = GetAbsoluteRead(Y,true);
                break;
            }
        case 0xDD:
            {
                ReadByte = GetAbsoluteRead(X,true);
                break;
            }
        default: ;
    }
    (A == ReadByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (A >= ReadByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(A - ReadByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

uint8 NesCPU::Inc(const uint8 Opcode, uint8 Reg) {
    Reg +=1;
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    return Reg;
}

uint8 NesCPU::Dec(const uint8 Opcode, uint8 Reg) {
    Reg -=1;
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    return Reg;
}

void NesCPU::Nop(uint8 Opcode) {
    //NO OPERATION.
}

void NesCPU::Lax(const uint8 Opcode) {
    switch(Opcode) {
        case 0xAF:
        case 0xBF:
            {
                //Absolute requires us to go back twice.
                const uint TempLastCycleCount = LastCycleCount;
                A = Ld(Opcode);
                PC--; 
                PC--; 
                X = Ld(Opcode);
                if(TempLastCycleCount != LastCycleCount) LastCycleCount--; //Remove double counted cycle
                break;
            }
        default:
            {
                const uint TempLastCycleCount = LastCycleCount;
                A = Ld(Opcode);
                PC--; // Go back once to load A
                X = Ld(Opcode);
                if(TempLastCycleCount != LastCycleCount) LastCycleCount--; //Remove double counted cycle
                break;
            }
    }
}

void NesCPU::Sax(const uint8 Opcode) {
    const uint8 ReadByte = static_cast<uint8>(A & X);
    switch(Opcode) {
        //(Indirect,X)
        case 0x83:
            {
                const unsigned short Address = GetIndirectAddress(X);
                m_mmu->Write(Address,ReadByte);
                break;
            }
        //Zero Page
        case 0x87:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        //Absolute
        case 0x8F:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                m_mmu->Write(Address,ReadByte);
                break;
            }
        //Zero Page Y
        case 0x97:
            {
                const unsigned short Address = m_mmu->Read(PC++) + Y;
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        default: ;
    }
}

void NesCPU::Dcp(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        //(Indirect,X)
        case 0xC3:
            {
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xC7:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0xCF:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xD3:
            {
                const unsigned short Address = GetIndirectIndexed(Y,false);
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xD7:
            {
                const unsigned short Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF)-1;
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0xDB:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + Y;
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xDF:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                ReadByte = m_mmu->Read(Address)-1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
    default: ;
    }
    (A == ReadByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (A >= ReadByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(A - ReadByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Isb(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        //(Indirect,X)
        case 0xE3:
            {
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xE7:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0xEF:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xF3:
            {
                const unsigned short Address = GetIndirectIndexed(Y,false);
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xF7:
            {
                const unsigned short Address = m_mmu->Read(PC++) + X;
                ReadByte = m_mmu->Read(Address & 0xFF)+1;
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0xFB:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + Y;
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0xFF:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                ReadByte = m_mmu->Read(Address)+1;
                m_mmu->Write(Address,ReadByte);
                break;
            }
    default: ;
    }
    const unsigned short Diff = static_cast<unsigned short>(A - ReadByte - (1 - P->ReadFlag(P->CFlag)));
    (static_cast<uint8>(Diff) == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (Diff <= 0xFF) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(Diff)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (((A ^ Diff) & (~ReadByte ^ Diff) & 0x80) != 0) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    A = static_cast<uint8>(Diff);
}

void NesCPU::Slo(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        case 0x03:
            {
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x07:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0x0F:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x13:
            {
                const unsigned short Address = GetIndirectIndexed(Y,false);
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x17:
            {
                const unsigned short Address = m_mmu->Read(PC++) + X;
                ReadByte = Asl(Opcode,m_mmu->Read(Address & 0xFF));
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0x1B:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + Y;
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x1F:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                ReadByte = Asl(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        default:;
    }
    A = static_cast<uint8>(A | ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Rla(const uint8 Opcode)
{
    uint8 ReadByte = 0x00;
    switch (Opcode)
    {
        case 0x23:
            {
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x27:
            {
                const unsigned short Address = m_mmu->Read(PC++);
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0x2F:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x33:
            {
                const unsigned short Address = GetIndirectIndexed(Y,false);
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x37:
            {
                const unsigned short Address = m_mmu->Read(PC++) + X;
                ReadByte = Rol(Opcode,m_mmu->Read(Address & 0xFF));
                m_mmu->Write(Address & 0xFF,ReadByte);
                break;
            }
        case 0x3B:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + Y;
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        case 0x3F:
            {
                const uint8 LowerByte = m_mmu->Read(PC++);
                const uint8 UpperByte = m_mmu->Read(PC++);
                const unsigned short Address = CombineBytePairIntoUShort(LowerByte,UpperByte) + X;
                ReadByte = Rol(Opcode,m_mmu->Read(Address));
                m_mmu->Write(Address,ReadByte);
                break;
            }
        default:;
    }
    A = static_cast<uint8>(A & ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}
