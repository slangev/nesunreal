// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCart.h"
#include "NesPPU.h"
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
	void AttachCart(shared_ptr<NesCart> cart);
	void AttachPPU(shared_ptr<NesPPU> ppu);
	void AttachController(shared_ptr<NesController> controller);
	bool RequestNMIInterrupt();
	uint8 Read(unsigned short Address) const;
	void Write(unsigned short Address, uint8 Data) const;
private:
	unique_ptr<vector<uint8>> m_cpuRam;
	shared_ptr<NesPPU> m_ppu;
	shared_ptr<NesCart> m_cart;
	shared_ptr<NesController> m_controller;
    uint m_memoryRamSize = 0x0800;
};
