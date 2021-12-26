// Fill out your copyright notice in the Description page of Project Settings.


#include "NesNoMapper.h"

NesNoMapper::NesNoMapper()
{
}

NesNoMapper::NesNoMapper(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory){
    this->PRGRomMemory = PRGRomMemory;
    this->PRGRamMemory = PRGRamMemory;
    this->ChrRomMemory = ChrRomMemory;
    this->ChrRamMemory = ChrRamMemory;
}

NesNoMapper::~NesNoMapper()
{
}

uint8 NesNoMapper::Read(unsigned short address) {
    return PRGRomMemory->at(address & 0x3FFF);
}

void NesNoMapper::Write(unsigned short address, uint8 data) {
    PRGRamMemory->at(address) = data;
}