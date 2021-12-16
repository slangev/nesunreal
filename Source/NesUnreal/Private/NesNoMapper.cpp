// Fill out your copyright notice in the Description page of Project Settings.


#include "NesNoMapper.h"

NesNoMapper::NesNoMapper()
{
}

NesNoMapper::NesNoMapper(shared_ptr<vector<uint8>> romMemory, shared_ptr<vector<uint8>> ramMemory, shared_ptr<vector<uint8>> vRomMemory){
    this->romMemory = romMemory;
    this->ramMemory = ramMemory;
    this->vRomMemory = vRomMemory;
}

NesNoMapper::~NesNoMapper()
{
}

uint8 NesNoMapper::Read(unsigned short address) {
    return romMemory->at(address & 0x3FFF);
}

void NesNoMapper::Write(unsigned short address, uint8 data) {
    ramMemory->at(address) = data;
}