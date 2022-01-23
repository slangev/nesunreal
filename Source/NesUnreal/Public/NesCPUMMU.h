// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCart.h"
#include "NesPPU.h"
#include "NesAPU.h"
#include "NesController.h"
#include <memory>
#include <vector>
/**
 * 
 */
class NESUNREAL_API NesCPUMMU
{

public:
	NesCPUMMU();
	~NesCPUMMU();
	void AttachCart(shared_ptr<NesCart> Cart);
	void AttachPpu(shared_ptr<NesPPU> Ppu);
	void AttachController(shared_ptr<NesController> Controller);
	void AttachApu(UNesApu *Apu);
	bool RequestNmiInterrupt() const;
	uint8 Read(unsigned short Address);
	void Write(unsigned short Address, uint8 Data);
	bool bOAMDMA = false;
 
private:
	unique_ptr<vector<uint8>> M_CPURam;
	shared_ptr<NesPPU> M_Ppu;
	shared_ptr<NesCart> M_Cart;
	shared_ptr<NesController> M_Controller;
	UNesApu *M_Sound;
    uint M_MemoryRamSize = 0x0800;
};
