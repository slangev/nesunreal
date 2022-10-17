// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include "NesCartController.h"
using namespace std;
/**
 * 
 */
class NESUNREAL_API NesCNROM : public NesCartController
{
public:
	NesCNROM();
	NesCNROM(shared_ptr<vector<uint8>> PrgRomMemory, shared_ptr<vector<uint8>> PrgRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory);
	~NesCNROM();
	virtual uint8 Read(unsigned short address) override;
	virtual void Write(unsigned short address, uint8 data) override;
	virtual uint8 GetMirrorMode() override;
	virtual bool GetIRQRequested() override;
	virtual void UpdateIRQCounter() override;
private:
	shared_ptr<vector<uint8>> PrgRomMemory;
    shared_ptr<vector<uint8>> PrgRamMemory;
    shared_ptr<vector<uint8>> ChrRomMemory;
	shared_ptr<vector<uint8>> ChrRamMemory;
	uint8 ChrBankSelect = 0;
	uint32 ROM_BANK_SIZE_8KB = 0x2000;
};
