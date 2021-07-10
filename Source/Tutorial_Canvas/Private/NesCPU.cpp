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
    streamPC << setfill ('0') << setw(sizeof(unsigned short)*2) << hex << PC-1;
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

void LogOpcode(FString msg, uint8 opcode) {
    string result = "";
    stringstream streamOPcode;
    streamOPcode << setfill ('0') << setw(sizeof(uint8)*2) << hex << (uint)opcode;
    result += streamOPcode.str();
    FString ueresult(result.c_str());
    ueresult = ueresult.ToUpper();
    UE_LOG(LogNesCPU, Log, TEXT("%s %s"),*msg, *ueresult);
}

uint NesCPU::Tick() {
    uint8 opcode = m_mmu->Read(PC++ & 0xFFFF);
    printNesTestLogLine(opcode);
    return handleInstructions(opcode);
}


uint NesCPU::handleInstructions(uint8 opcode) {
        uint lastCycleCount = cycleCount[opcode];
        switch(opcode) {
            case 0x01:
                ORA(opcode);
                break;
            case 0x08:
                PHP(opcode);
                break;
            case 0x09:
                ORA(opcode);
                break;
            case 0x0A:
                A = ASL(opcode,A);
                break;
            case 0x10:
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->NFlag) == 0);
                break;
            case 0x18:
                CLC(opcode);
                break;
            case 0x20:
                JSR(opcode);
                break;
            case 0x21:
                AND(opcode);
                break;
            case 0x24:
                BIT(opcode);
                break;
            case 0x28:
                PLP(opcode);
                break;
            case 0x29:
                AND(opcode);
                break;
            case 0x2A:
                A = ROL(opcode,A);
                break;
            case 0x30:
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->NFlag) == 1);
                break;
            case 0x38:
                SEC(opcode);
                break;
            case 0x40:
                RTI(opcode);
                break;
            case 0x41:
                EOR(opcode);
                break;
            case 0x48:
                PHA(opcode);
                break;
            case 0x49:
                EOR(opcode);
                break;
            case 0x4A:
                A = LSR(opcode,A);
                break;
            case 0x4C:
                JMP(opcode);
                break;
            case 0x50:
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->VFlag) == 0);
                break;
            case 0x60:
                RTS(opcode);
                break;
            case 0x61:
                ADC(opcode);
                break;
            case 0x68:
                PLA(opcode);
                break;
            case 0x69:
                ADC(opcode);
                break;
            case 0x6A:
                A = ROR(opcode,A);
                break;
            case 0x70:
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->VFlag) == 1);
                break;
            case 0x78:
                SEI(opcode);
                break;
            case 0x81:
                STORE(opcode,X);
                break;
            case 0x85:
                STORE(opcode,A);
                break;
            case 0x86:
                STORE(opcode,X);
                break;
            case 0x88:
                Y = DEC(opcode,Y);
                break;
            case 0x8A:
                A = TRANSFER(opcode,X);
                break;
            case 0x8D:
                STORE(opcode,A);
                break;
            case 0x8E:
                STORE(opcode,X);
                break;
            case 0x90:
                //BCC
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->CFlag) == 0);
                break;
            case 0x98:
                A = TRANSFER(opcode,Y);
                break;
            case 0x9A:
                SP = TRANSFER(opcode,X);
                break;
            case 0xA0:
                Y = LD(opcode);
                break;
            case 0xA1:
                A = LD(opcode);
                break;
            case 0xA2:
                X = LD(opcode);
                break;
            case 0xA4:
                Y = LD(opcode);
                break;
            case 0xA5:
                A = LD(opcode);
                break;
            case 0xA8:
                Y = TRANSFER(opcode,A);
                break;
            case 0xA9:
                A = LD(opcode);
                break;
            case 0xAA:
                X = TRANSFER(opcode,A);
                break;
            case 0xAD:
                A = LD(opcode);
                break;
            case 0xAE:
                X = LD(opcode);
                break;
            case 0xB0:
                //BCS
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->CFlag) == 1);
                break;
            case 0xB8:
                CLV(opcode);
                break;
            case 0xBA:
                X = TRANSFER(opcode,SP);
                break;
            case 0xC0:
                CP(opcode,Y);
                break;
            case 0xC1:
                CMP(opcode);
                break;
            case 0xC8:
                Y = INC(opcode,Y);
                break;
            case 0xC9:
                CMP(opcode);
                break;
            case 0xCA:
                X = DEC(opcode,X);
                break;
            case 0xD0:
                //BNE
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->ZFlag) == 0);
                break;
            case 0xD8:
                CLD(opcode);
                break;
            case 0xE0:
                CP(opcode,X);
                break;
            case 0xE1:
                SBC(opcode);
                break;
            case 0xE9:
                SBC(opcode);
                break;
            case 0xEA:
                NOP(opcode);
                break;
            case 0xE8:
                X = INC(opcode,X);
                break;
            case 0xF0:
                //BEQ
                lastCycleCount += BRANCH(opcode,P->ReadFlag(P->ZFlag) == 1);
                break;
            case 0xF8:
                SED(opcode);
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

unsigned short NesCPU::combineBytePairIntoUShort(uint8 lsb, uint8 msb) {
    unsigned short result = ((msb << 8) | lsb);
    return result;
}



// Item1 = MSB and Item 2 = LSB
void NesCPU::separateWordToBytes(ushort word, uint8 result[]) {
    result[0] = ((word & 0xFF00) >> 8);
    result[1] = (word & 0x00FF);
}

uint8 NesCPU::getBit(uint8 pos, uint8 reg) {
    uint8 result = (uint8)((reg & (1 << pos)));
    result = (uint8)(result >> pos); 
    return result;
}

uint8 NesCPU::setBit(uint8 pos, uint8 r) {
    uint8 result = (uint8)(r | (1 << pos));
    return result;
}

uint8 NesCPU::resetBit(uint8 pos, uint8 r) {
    uint8 result = (uint8)((r  & ~(1 << pos)));
    return result;
}

// The 6502 only supported Indirect Addressing using X,Y registers
unsigned short NesCPU::getIndirectAddress(uint8 reg) {
    uint8 lowerByte = m_mmu->Read(PC++);
    lowerByte += reg;
    uint8 addressLsb = m_mmu->Read(lowerByte); // Read the lsb of our target address.
    uint8 addressMsb = m_mmu->Read(lowerByte+1); // Read the msb of our target address.
    unsigned short address = combineBytePairIntoUShort(addressLsb,addressMsb);
    return address;
}

void NesCPU::JMP(uint8 opcode) {
    //Absolute address mode
    if(opcode == 0x4C) {
        uint8 lsb = m_mmu->Read(PC++);
        uint8 msb = m_mmu->Read(PC++);
        PC = combineBytePairIntoUShort(lsb,msb);
    }
}

void NesCPU::JSR(uint8 opcode) {
    uint8 lowByte = m_mmu->Read(PC++);
    uint8 highByte = m_mmu->Read(PC++);    
    unsigned short addr = combineBytePairIntoUShort(lowByte,highByte);
    uint8 result[2];
    separateWordToBytes((unsigned short)(PC-1), result);
    m_mmu->Write(SP--|0x100,result[0]);
    m_mmu->Write(SP--|0x100,result[1]);
    PC = addr;
}

//The RTS instruction is used at the end of a subroutine to return to the calling routine. It pulls the program counter from the stack.
void NesCPU::RTS(uint8 opcode) {
    uint8 lowByte = m_mmu->Read(++SP|0x100);
    uint8 highByte = m_mmu->Read(++SP|0x100);
    unsigned short addr = combineBytePairIntoUShort(lowByte,highByte);
    PC = (unsigned)(addr+1);
}

void NesCPU::PHA(uint8 opcode) {
    m_mmu->Write(SP--|0x100,A);
}

void NesCPU::PLP(uint8 opcode) {
    uint8 readByte = m_mmu->Read(++SP|0x100);
    uint8 currPState = P->pStateWithBFlag();
    P->pSetState(readByte);
    (getBit(5,currPState) == 1) ? P->SetFlag(P->XFlag) : P->ResetFlag(P->XFlag);
    (getBit(4,currPState) == 1) ? P->SetFlag(P->BFlag) : P->ResetFlag(P->BFlag);
}

void NesCPU::RTI(uint8 opcode) {
    PLP(opcode); 
    RTS(opcode); 
    PC -=1;      
}

uint8 NesCPU::LD(uint8 opcode) {
    uint8 readByte = 0;
    unsigned short address = 0;
    switch(opcode) {
        //Immediate page
        case 0xA0:
        case 0xA2:
        case 0xA9:
            readByte = m_mmu->Read(PC++);
            break;
        //Zero Page
        case 0xA4:
        case 0xA5:
            address = m_mmu->Read(PC++);
            readByte = m_mmu->Read(address);
            break;
        //Absolute
        case 0xAD:
        case 0xAE: {
            uint8 lowerByte = m_mmu->Read(PC++);
            uint8 upperByte = m_mmu->Read(PC++);
            address = combineBytePairIntoUShort(lowerByte,upperByte);
            readByte = m_mmu->Read(address);
            break;
        }
        //Indirect,X
        case 0xA1:
            address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        default:
            //Debug.LogError("Bad LD operation");
            return 0xFF;
    }
    (readByte == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,readByte) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    return readByte;
}

void NesCPU::CP(uint8 opcode, uint8 reg) {
    switch(opcode) {
        case 0xC0:
        case 0xE0:
            uint8 readByte = m_mmu->Read(PC++);
            (reg == readByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (reg >= readByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
            (getBit(7,(uint8)(reg - readByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            break;
    }
}

void NesCPU::BIT(uint8 opcode) {
    //Zero-page
    if(opcode == 0x24) {
        uint8 memoryLocation = m_mmu->Read(PC++);
        uint8 readByte = m_mmu->Read(memoryLocation & 0xFF);
        uint8 result = (uint8)(A & readByte);
        if(result == 0) {
            P->SetFlag(P->ZFlag);
        }
        uint8 NflagByte = getBit(P->NFlag,readByte);
        uint8 VflagByte = getBit(P->VFlag,readByte);
        (NflagByte == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
        (VflagByte == 1) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    }
}

void NesCPU::STORE(uint8 opcode, uint8 reg) {
    switch(opcode) {
        // Indirect X
        case 0x81:{
            unsigned short address = getIndirectAddress(X);
            m_mmu->Write(address,A);
            break;
        } 
        // Zero-page
        case 0x85:
        case 0x86: {
            uint8 lsb = m_mmu->Read(PC++);
            m_mmu->Write(lsb & 0xFF,reg);
            break;
        }
        // Absolute
        case 0x8D:
        case 0x8E:{
            uint8 lsb = m_mmu->Read(PC++);
            uint8 msb = m_mmu->Read(PC++);
            unsigned short address = combineBytePairIntoUShort(lsb,msb);
            m_mmu->Write(address,reg);
        }
        break;
    }
}

void NesCPU::AND(uint8 opcode) {
    uint8 readByte = 0x00;
    switch(opcode) {
        //Indirect X
        case 0x21:{
            ushort address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        } 
        case 0x29: {
            readByte = m_mmu->Read(PC++);
            break;
        } 
    }
    A = (uint8)(A & readByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

uint8 NesCPU::LSR(uint8 opcode, uint8 reg) {
    uint8 oldBit = getBit(0,reg);
    reg = (uint8)(reg >> 1);
    (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (oldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return reg;
}

uint8 NesCPU::ASL(uint8 opcode, uint8 reg) {
    uint8 oldBit = getBit(7,reg);
    reg = (uint8)(reg << 1);
    (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (oldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return reg;
}

uint8 NesCPU::ROR(uint8 opcode, uint8 reg) {
    uint8 oldBit = getBit(0,reg);
    reg = (uint8)(reg >> 1);
    reg = (P->ReadFlag(P->CFlag) == 1) ? setBit(7,reg) : resetBit(7,reg);
    (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (oldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return reg;
}

uint8 NesCPU::ROL(uint8 opcode, uint8 reg) {
    uint8 oldBit = getBit(7,reg);
    reg = (uint8)(reg << 1);
    reg = (P->ReadFlag(P->CFlag) == 1) ? setBit(0,reg) : resetBit(0,reg);
    (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (oldBit == 1) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    return reg;
}

void NesCPU::ADC(uint8 opcode) {
    uint8 readByte = 0x00;
    ushort sum = 0x0000;
    switch(opcode) {
        case 0x61:{
            ushort address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        }
        case 0x69:{
            readByte = m_mmu->Read(PC++);
            break;
        }  
    }
    sum = (ushort)(A + readByte + P->ReadFlag(P->CFlag));
    ((uint8)(sum) == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (sum > 0xFF) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (getBit(7,(uint8)(sum)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (((A ^ sum) & (readByte ^ sum) & 0x80) != 0) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    A = (uint8)(sum);
}

void NesCPU::ORA(uint8 opcode) {
    uint8 readByte = 0x00;
    switch(opcode) {
        case 0x01: {
            unsigned short address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        }
        case 0x09: {
            readByte = m_mmu->Read(PC++);
            break;
        }  
    }
    A = (uint8)(A | readByte);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::EOR(uint8 opcode) {
        uint8 readByte = 0x00;
        switch(opcode) {
            case 0x41:{
                unsigned short address = getIndirectAddress(X);
                readByte = m_mmu->Read(address);
                break;
            }  
            case 0x49:
                readByte = m_mmu->Read(PC++);
                break;
        }
        A = (uint8)(A ^ readByte);
        (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
        (getBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    }

//https://wiki.nesdev.com/w/index.php/Status_flags
void NesCPU::PHP(uint8 opcode) {
    uint8 copyP = P->pStateWithBFlag();
    copyP = (uint8)(copyP | 0x30);
    m_mmu->Write(SP--|0x100,copyP);
}

uint NesCPU::BRANCH(uint8 opcode, bool cc) {
    int8 sb = (uint8)(m_mmu->Read(PC++));
    //int8 sb = unchecked((sbyte)(b));
    if(cc) {
        uint8 prevPage = (uint8)((PC & 0xFF00) >> 8);
        PC = (unsigned short)(PC + sb);
        uint8 currPage = (uint8)((PC & 0xFF00) >> 8); 
        return (uint)((currPage != prevPage) ? 2 : 1); // Taken branch and page crossed equals 2. If we take a branch and don't cross pages then return 1.
    }
    return 0;
}

void NesCPU::SBC(uint8 opcode) {
    uint8 readByte = 0x00;
    unsigned short diff = 0x000;
    switch(opcode) {
        case 0xE1:{
            unsigned short address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        }
        case 0xE9:
            readByte = m_mmu->Read(PC++);
            break;
    }
    diff = (unsigned short)(A - readByte - (1 - P->ReadFlag(P->CFlag)));
    ((uint8)(diff) == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (diff <= 0xFF) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (getBit(7,(uint8)(diff)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
    (((A ^ diff) & (~readByte ^ diff) & 0x80) != 0) ? P->SetFlag(P->VFlag) : P->ResetFlag(P->VFlag);
    A = (uint8)(diff);
}

void NesCPU::PLA(uint8 opcode) {
    A = m_mmu->Read(++SP|0x100);
    (A == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (getBit(7,A) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

void NesCPU::SEC(uint8 opcode) {
    P->SetFlag(P->CFlag);
}

void NesCPU::SEI(uint8 opcode) {
    P->SetFlag(P->IFlag);
}

void NesCPU::SED(uint8 opcode) {
    P->SetFlag(P->DFlag);
}

void NesCPU::CLC(uint8 opcode) {
    P->ResetFlag(P->CFlag);
}

void NesCPU::CLD(uint8 opcode) {
    P->ResetFlag(P->DFlag);
}

void NesCPU::CLV(uint8 opcode) {
    P->ResetFlag(P->VFlag);
}


uint8 NesCPU::TRANSFER(uint8 opcode, uint8 reg) {
    switch(opcode) {
        case 0x9A: 
            return reg;
        default:
            (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return reg;
    }
}

void NesCPU::CMP(uint8 opcode) {
    uint8 readByte = 0x00;
    switch(opcode) {
        case 0xC1: {
            unsigned short address = getIndirectAddress(X);
            readByte = m_mmu->Read(address);
            break;
        }
        case 0xC9:
            readByte = m_mmu->Read(PC++);
            break;
    }
    (A == readByte) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
    (A >= readByte) ? P->SetFlag(P->CFlag) : P->ResetFlag(P->CFlag);
    (getBit(7,(uint8)(A-readByte)) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
}

uint8 NesCPU::INC(uint8 opcode, uint8 reg) {
    switch(opcode) {
        case 0xC8:
        case 0xE8:{
            reg +=1;
            (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return reg;
        }
        default:
            //Debug.LogError("Bad INC operation");
            return 0xFF;
    }
}

uint8 NesCPU::DEC(uint8 opcode, uint8 reg) {
    switch(opcode) {
        case 0x88:
        case 0xCA:
            reg -=1;
            (reg == 0) ? P->SetFlag(P->ZFlag) : P->ResetFlag(P->ZFlag);
            (getBit(7,reg) == 1) ? P->SetFlag(P->NFlag) : P->ResetFlag(P->NFlag);
            return reg;
        default:
            //Debug.LogError("Bad INC operation");
            return 0xFF;
    }
}

void NesCPU::NOP(uint8 opcode) {
    //NO OPERATION.
}