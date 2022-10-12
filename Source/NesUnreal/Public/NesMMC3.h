// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <memory>
#include "NesCartController.h"
#include "CoreMinimal.h"

/**
 * 
 */

class NesMMC3BankSelectRegister
{
public:
	NesMMC3BankSelectRegister(){};
	~NesMMC3BankSelectRegister(){};
	uint8 Read() { return this->Data; };
	void Write(uint8 InputData) 
	{ 
		this->Data = InputData;
		this->BankRegister = (InputData & 0x7);
		this->PRGROMBankMode = (InputData & 0x40) >> 6;
		this->CHRROMBankMode = (InputData & 0x80) >> 7;
	};
	uint8 GetBankRegister() { return BankRegister; };
	uint8 GetPRGROMBankMode() { return PRGROMBankMode; };
	uint8 GetCHRROMBankMode() { return CHRROMBankMode; };
private:
	uint8 Data = 0x00;
	uint8 BankRegister = 0x00;
	uint8 PRGROMBankMode = 0x00;
	uint8 CHRROMBankMode = 0x00;
};

class NESUNREAL_API NesMMC3 : public NesCartController
{
public:
	NesMMC3();
	NesMMC3(std::shared_ptr<std::vector<uint8>> PrgRomMemory, std::shared_ptr<std::vector<uint8>> PrgRamMemory, std::shared_ptr<std::vector<uint8>> ChrRomMemory, std::shared_ptr<std::vector<uint8>> ChrRamMemory, bool bBatteryBacked);
	~NesMMC3();
	virtual uint8 Read(unsigned short Address) override;
	virtual void Write(unsigned short Address, uint8 Data) override;
	virtual uint8 GetMirrorMode() override;
private:
	std::shared_ptr<std::vector<uint8>> PrgRomMemory;
    std::shared_ptr<std::vector<uint8>> PrgRamMemory;
    std::shared_ptr<std::vector<uint8>> ChrRomMemory;
	std::shared_ptr<std::vector<uint8>> ChrRamMemory;
	uint32 BANK_SIZE_8KB = 0x2000;
	uint32 BANK_SIZE_2KB = 0x800;
	uint32 BANK_SIZE_1KB = 0x400;

	bool bBatteryBacked = false;

	/*
		The MMC3 has 4 pairs of registers at $8000-$9FFF, $A000-$BFFF, $C000-$DFFF, and $E000-$FFFF - even addresses ($8000, $8002, etc.) select the low register and odd addresses ($8001, $8003, etc.) select the high register in each pair. These can be broken into two independent functional units: memory mapping ($8000, $8001, $A000, $A001) and scanline counting ($C000, $C001, $E000, $E001). 
	*/
	std::unique_ptr<NesMMC3BankSelectRegister> BankSelect;
	std::vector<uint8> ChrBankData; // R0-R5
	std::vector<uint8> PrgBankData; // R6-R7
	uint8 MirrorMode = 0;
	uint8 PrgRamProtect = 0;
	uint8 IRQLatch = 0;
	uint8 IRQReload = 0;
	uint8 OpenBusData = 0;
	bool bIsIRQEnabled = false;
	bool bIRQReloadRequest = false;
};
