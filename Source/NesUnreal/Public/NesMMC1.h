// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesCartController.h"
#include <vector>
#include <memory>
#include "CoreMinimal.h"
using namespace std;
/**
 * 
 */

class NesMMC1ControlRegister
{
public:
	NesMMC1ControlRegister(){};
	~NesMMC1ControlRegister(){};
	uint8 Read() { return M_Data; };
	void Write(uint8 Data) { 
		M_Data = Data;
		M_MirrorMode = Data & 0x03;
		M_PRGROMBankMode = Data & 0x0C;
		M_CHRROMBankMode = Data & 0x10;
	 };
	uint8 GetMirrorMode() { return M_MirrorMode; };
	uint8 GetPRGROMBankMode() { return M_PRGROMBankMode; };
	uint8 GetCHRROMBankMode() { return M_CHRROMBankMode; };
private:
	uint8 M_Data = 0x00;
	uint8 M_MirrorMode = 0x00;
	uint8 M_PRGROMBankMode = 0x00;
	uint8 M_CHRROMBankMode = 0x00;
};

class NESUNREAL_API NesMMC1 : public NesCartController
{
public:
	NesMMC1();
	NesMMC1(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory);
	~NesMMC1();
	virtual uint8 Read(unsigned short Address) override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual uint8 GetMirrorMode() override;
	enum RegisterSelector : uint8 {
		ControlRegisterEnum = 0,
		ChrBank0RegisterEnum,
		ChrBank1RegisterEnum,
		PrgBankRegisterEnum
	};
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
	uint32 ROM_BANK_SIZE_32KB = 0x8000;
	uint32 ROM_BANK_SIZE_16KB = 0x4000;
	uint32 ROM_BANK_SIZE_8KB = 0x2000;
	uint32 ROM_BANK_SIZE_4KB = 0x1000;
};
