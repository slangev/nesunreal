// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <memory>
#include "NesCartController.h"
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
	uint32 ROM_BANK_SIZE_8KB = 0x2000;
	uint32 ROM_BANK_SIZE_4KB = 0x1000;
	uint8 PRGROMBankSelect = 0x00;
	uint8 CHRROMFD0000 = 0x00;
	uint8 CHRROMFE0000 = 0x00;
	uint8 CHRROMFD1000 = 0x00;
	uint8 CHRROMFE1000 = 0x00;
	uint8 Latch0 = 0xFE;
	uint8 Latch1 = 0xFE;
	uint8 MirrorMode = 0;
	bool bBatteryBacked = false;
};
