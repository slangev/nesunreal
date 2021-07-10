// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCart.h"
#include <memory>
#include <vector>

/**
 * 
 */
class TUTORIAL_CANVAS_API NesMMU
{
public:
	NesMMU();
	~NesMMU();
	void AttachCart(unique_ptr<NesCart> cart);
	//void AttachCart(shared_ptr<NesCart> cart);
	uint8 Read(unsigned short address);
	void Write(unsigned short address, uint8 data);
private:
	unique_ptr<vector<uint8>> m_cpuRam;
	unique_ptr<vector<uint8>> m_ppuRam;
	unique_ptr<NesCart> m_cart;
    uint m_memorySize = 0x0800;
};
