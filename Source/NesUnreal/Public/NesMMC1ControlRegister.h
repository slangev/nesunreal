// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NesMMCRegister.h"
#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesMMC1ControlRegister : NesMMCRegister
{
public:
	NesMMC1ControlRegister();
	~NesMMC1ControlRegister();
	virtual uint8 Read() override;
	virtual void Write(uint8 Data) override;
	uint8 GetMirrorMode() {return M_MirrorMode;};
	uint8 GetPRGROMBankMode() {return M_PRGROMBankMode;};
	uint8 GetCHRROMBankMode() {return M_CHRROMBankMode;};
private:
	uint8 M_Data = 0x00;
	uint8 M_MirrorMode = 0x00;
	uint8 M_PRGROMBankMode = 0x00;
	uint8 M_CHRROMBankMode = 0x00;
};
