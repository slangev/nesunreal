// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesCartController.h"
#include "NesMMC1ControlRegister.h"
#include <vector>
#include <memory>
#include "CoreMinimal.h"
using namespace std;
/**
 * 
 */
class NESUNREAL_API NesMMC1 : public NesCartController
{
public:
	NesMMC1();
	NesMMC1(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory);
	~NesMMC1();
	virtual uint8 Read(unsigned short Address) override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual uint8 GetMirrorMode() override;
private:
	shared_ptr<vector<uint8>> PRGRomMemory;
    shared_ptr<vector<uint8>> PRGRamMemory;
    shared_ptr<vector<uint8>> ChrRomMemory;
	shared_ptr<vector<uint8>> ChrRamMemory;
	uint8 LoadRegister = 0;
	uint8 ShiftRegister = 0x10; // the 1 bit will be used to detech if SR full
	unique_ptr<NesMMC1ControlRegister> ControlRegister;
	uint8 ChrBank0Register = 0;
	uint8 ChrBank1Register = 0;
	uint8 PrgBankRegister = 0;
};
