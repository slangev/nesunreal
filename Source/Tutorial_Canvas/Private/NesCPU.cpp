// Fill out your copyright notice in the Description page of Project Settings.


#include "NesCPU.h"

DEFINE_LOG_CATEGORY(LogNesCPU);


const uint NesCPU::cycleCount[] = {
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
    totalCycles = 7;
}

NesCPU::~NesCPU()
{
}

void NesCPU::PrintNesTestLogLine(uint8 Opcode) {
    string Result = to_string(lineNumber++) + " ";
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
    Result += to_string(totalCycles);

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
    uint8 const opcode = m_mmu->Read(PC++ & 0xFFFF);
    PrintNesTestLogLine(opcode);
    return HandleInstructions(opcode);
}


uint NesCPU::HandleInstructions(uint8 opcode) {
        uint lastCycleCount = cycleCount[opcode];
        switch(opcode) {
            case 0x01:
                Ora(opcode);
                break;
            case 0x05:
                Ora(opcode);
                break;
            case 0x08:
                Php(opcode);
                break;
            case 0x09:
                Ora(opcode);
                break;
            case 0x0A:
                A = Asl(opcode,A);
                break;
            case 0x10:
                lastCycleCount += Branch(opcode,P->ReadFlag(P->NFlag) == 0);
                break;
            case 0x18:
                Clc(opcode);
                break;
            case 0x20:
                Jsr(opcode);
                break;
            case 0x21:
                And(opcode);
                break;
            case 0x24:
                Bit(opcode);
                break;
            case 0x28:
                Plp(opcode);
                break;
            case 0x29:
                And(opcode);
                break;
            case 0x2A:
                A = Rol(opcode,A);
                break;
            case 0x30:
                lastCycleCount += Branch(opcode,P->ReadFlag(P->NFlag) == 1);
                break;
            case 0x38:
                Sec(opcode);
                break;
            case 0x40:
                Rti(opcode);
                break;
            case 0x41:
                Eor(opcode);
                break;
            case 0x48:
                Pha(opcode);
                break;
            case 0x49:
                Eor(opcode);
                break;
            case 0x4A:
                A = Lsr(opcode,A);
                break;
            case 0x4C:
                Jmp(opcode);
                break;
            case 0x50:
                lastCycleCount += Branch(opcode,P->ReadFlag(P->VFlag) == 0);
                break;
            case 0x60:
                Rts(opcode);
                break;
            case 0x61:
                Adc(opcode);
                break;
            case 0x68:
                Pla(opcode);
                break;
            case 0x69:
                Adc(opcode);
                break;
            case 0x6A:
                A = Ror(opcode,A);
                break;
            case 0x70:
                lastCycleCount += Branch(opcode,P->ReadFlag(P->VFlag) == 1);
                break;
            case 0x78:
                Sei(opcode);
                break;
            case 0x81:
                Store(opcode,X);
                break;
            case 0x84:
                Store(opcode,Y);
                break;
            case 0x85:
                Store(opcode,A);
                break;
            case 0x86:
                Store(opcode,X);
                break;
            case 0x88:
                Y = Dec(opcode,Y);
                break;
            case 0x8A:
                A = Transfer(opcode,X);
                break;
            case 0x8D:
                Store(opcode,A);
                break;
            case 0x8E:
                Store(opcode,X);
                break;
            case 0x90:
                //BCC
                lastCycleCount += Branch(opcode,P->ReadFlag(P->CFlag) == 0);
                break;
            case 0x98:
                A = Transfer(opcode,Y);
                break;
            case 0x9A:
                SP = Transfer(opcode,X);
                break;
            case 0xA0:
                Y = Ld(opcode);
                break;
            case 0xA1:
                A = Ld(opcode);
                break;
            case 0xA2:
                X = Ld(opcode);
                break;
            case 0xA4:
                Y = Ld(opcode);
                break;
            case 0xA5:
                A = Ld(opcode);
                break;
            case 0xA6:
                X = Ld(opcode);
                break;
            case 0xA8:
                Y = Transfer(opcode,A);
                break;
            case 0xA9:
                A = Ld(opcode);
                break;
            case 0xAA:
                X = Transfer(opcode,A);
                break;
            case 0xAD:
                A = Ld(opcode);
                break;
            case 0xAE:
                X = Ld(opcode);
                break;
            case 0xB0:
                //BCS
                lastCycleCount += Branch(opcode,P->ReadFlag(P->CFlag) == 1);
                break;
            case 0xB8:
                Clv(opcode);
                break;
            case 0xBA:
                X = Transfer(opcode,SP);
                break;
            case 0xC0:
                Cp(opcode,Y);
                break;
            case 0xC1:
                Cmp(opcode);
                break;
            case 0xC8:
                Y = Inc(opcode,Y);
                break;
            case 0xC9:
                Cmp(opcode);
                break;
            case 0xCA:
                X = Dec(opcode,X);
                break;
            case 0xD0:
                //BNE
                lastCycleCount += Branch(opcode,P->ReadFlag(P->ZFlag) == 0);
                break;
            case 0xD8:
                Cld(opcode);
                break;
            case 0xE0:
                Cp(opcode,X);
                break;
            case 0xE1:
                Sbc(opcode);
                break;
            case 0xE9:
                Sbc(opcode);
                break;
            case 0xEA:
                Nop(opcode);
                break;
            case 0xE8:
                X = Inc(opcode,X);
                break;
            case 0xF0:
                //BEQ
                lastCycleCount += Branch(opcode,P->ReadFlag(P->ZFlag) == 1);
                break;
            case 0xF8:
                Sed(opcode);
                break;
            default:
                LogOpcode("Unknown opcode: ", opcode);
                break;
        }
        totalCycles += lastCycleCount;
        return lastCycleCount; 
}

void NesCPU::AttachMemory(shared_ptr<NesMMU> mmu, unsigned short startPC) {
    this->m_mmu = mmu;
    this->PC = (startPC == 0) ? ((m_mmu->Read(0xFFFD) << 8) | m_mmu->Read(0xFFFC)) : startPC;
}

unsigned short NesCPU::CombineBytePairIntoUShort(uint8 lsb, uint8 msb) {
    const unsigned short Result = ((msb << 8) | lsb);
    return Result;
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
    const uint8 AddressMsb = m_mmu->Read(LowerByte+1); // Read the msb of our target address.
    const unsigned short Address = CombineBytePairIntoUShort(AddressLsb,AddressMsb);
    return Address;
}

void NesCPU::Jmp(const uint8 Opcode) {
    //Absolute address mode
    if(Opcode == 0x4C) {
        const uint8 Lsb = m_mmu->Read(PC++);
        const uint8 Msb = m_mmu->Read(PC++);
        PC = CombineBytePairIntoUShort(Lsb,Msb);
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
        case 0xA6: {
                Address = m_mmu->Read(PC++);
                ReadByte = m_mmu->Read(Address & 0xFF);
                break;
            }
        //Absolute
        case 0xAD:
        case 0xAE: {
            const uint8 LowerByte = m_mmu->Read(PC++);
            const uint8 UpperByte = m_mmu->Read(PC++);
            Address = CombineBytePairIntoUShort(LowerByte,UpperByte);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        //Indirect,X
        case 0xA1: {
                Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address);
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
    switch(Opcode) {
        case 0xC0:
        case 0xE0:
            {
                const uint8 ReadByte = m_mmu->Read(PC++);
                (Reg == ReadByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
                (Reg >= ReadByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
                (GetBit(7,static_cast<uint8>(Reg - ReadByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
                break;
            }
        default:
            {
                
            }
    }
}

void NesCPU::Bit(const uint8 Opcode) {
    //Zero-page
    if(Opcode == 0x24) {
        const uint8 MemoryLocation = m_mmu->Read(PC++);
        const uint8 ReadByte = m_mmu->Read(MemoryLocation & 0xFF);
        if(const uint8 Result = static_cast<uint8>(A & ReadByte); Result == 0) {
            P->SetFlag(P->ZFlag);
        }
        const uint8 NFlagByte = GetBit(P->NFlag,ReadByte);
        const uint8 VFlagByte = GetBit(P->VFlag,ReadByte);
        (NFlagByte == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
        (VFlagByte == 1) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    }
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
            const uint8 Lsb = m_mmu->Read(PC++);
            m_mmu->Write(Lsb & 0xFF,Reg);
            break;
        }
        // Absolute
        case 0x8D:
        case 0x8E:{
            const uint8 Lsb = m_mmu->Read(PC++);
            const uint8 Msb = m_mmu->Read(PC++);
            const unsigned short Address = CombineBytePairIntoUShort(Lsb,Msb);
            m_mmu->Write(Address,Reg);
        }
        break;
    default: ;
    }
}

void NesCPU::And(const uint8 Opcode) {
    uint8 ReadByte = 0x00;
    switch(Opcode) {
        //Indirect X
        case 0x21:{
            const ushort Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        } 
        case 0x29: {
            ReadByte = m_mmu->Read(PC++);
            break;
        }
    default: ;
    }
    A = static_cast<uint8>(A & ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    
}

uint8 NesCPU::Lsr(uint8 Opcode, uint8 Reg) const {
    const uint8 OldBit = GetBit(0,Reg);
    Reg = static_cast<uint8>(Reg >> 1);
    (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (OldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return Reg;
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

void NesCPU::Adc(uint8 opcode) {
    uint8 ReadByte = 0x00;
    switch(opcode) {
        case 0x61:{
            const unsigned short Address = GetIndirectAddress(X);
            ReadByte = m_mmu->Read(Address);
            break;
        }
        case 0x69:{
            ReadByte = m_mmu->Read(PC++);
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

void NesCPU::Ora(uint8 opcode) {
    uint8 ReadByte = 0x00;
    switch(opcode) {
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
    default: ;
    }
    A = static_cast<uint8>(A | ReadByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (GetBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::Eor(uint8 opcode) {
        uint8 ReadByte = 0x00;
        switch(opcode) {
            case 0x41:{
                const unsigned short Address = GetIndirectAddress(X);
                ReadByte = m_mmu->Read(Address);
                break;
            }  
            case 0x49:
                ReadByte = m_mmu->Read(PC++);
                break;
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
        const uint8 currPage = static_cast<uint8>((PC & 0xFF00) >> 8); 
        return static_cast<uint>((currPage != PrevPage) ? 2 : 1); // Taken branch and page crossed equals 2. If we take a branch and don't cross pages then return 1.
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
        case 0xE9:
            ReadByte = m_mmu->Read(PC++);
            break;
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
        case 0xC9:
            ReadByte = m_mmu->Read(PC++);
            break;
        default: ;
    }
    (A == ReadByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (A >= ReadByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (GetBit(7,static_cast<uint8>(A - ReadByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

uint8 NesCPU::Inc(const uint8 Opcode, uint8 Reg) const {
    switch(Opcode) {
        case 0xC8:
        case 0xE8:{
            Reg +=1;
            (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return Reg;
        }
        default:
            //Debug.LogError("Bad INC operation");
            return 0xFF;
    }
}

uint8 NesCPU::Dec(const uint8 Opcode, uint8 Reg) const {
    switch(Opcode) {
        case 0x88:
        case 0xCA:
            Reg -=1;
            (Reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (GetBit(7,Reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return Reg;
        default:
            //Debug.LogError("Bad INC operation");
            return 0xFF;
    }
}

void NesCPU::Nop(uint8 Opcode) {
    //NO OPERATION.
}