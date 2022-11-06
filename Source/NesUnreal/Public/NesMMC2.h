// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesCartController.h"
#include <vector>
#include <memory>
#include "NesCPU.h"
#include "CoreMinimal.h"
using namespace std;
/**
 * 
 */
class NESUNREAL_API NesMMC2 : public NesCartController
{
public:
	NesMMC2();
	NesMMC2(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory, bool bBatteryBacked);
	~NesMMC2();
	virtual uint8 Read(unsigned short Address) override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual uint8 GetMirrorMode() override;
	virtual bool GetIRQRequested() override;
	virtual void UpdateIRQCounter() override;
private:
	shared_ptr<vector<uint8>> PrgRomMemory;
    shared_ptr<vector<uint8>> PrgRamMemory;
    shared_ptr<vector<uint8>> ChrRomMemory;
	shared_ptr<vector<uint8>> ChrRamMemory;
	uint8 LoadRegister = 0;
	uint8 ShiftRegister = 0x10; // the 1 bit will be used to detech if SR full
	uint8 ChrBank0Register = 0;
	uint8 ChrBank1Register = 0;
	uint8 PrgBankRegister = 0;
	uint32 ROM_BANK_SIZE_32KB = 0x8000;
	uint32 ROM_BANK_SIZE_16KB = 0x4000;
	uint32 ROM_BANK_SIZE_8KB = 0x2000;
	uint32 ROM_BANK_SIZE_4KB = 0x1000;
	bool bBatteryBacked = false;
};
