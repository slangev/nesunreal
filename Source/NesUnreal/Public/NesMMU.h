// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCart.h"
#include <memory>
#include <vector>

/**
 * 
 */
class NESUNREAL_API NesMMU
{
public:
	NesMMU();
	~NesMMU();
	void AttachCart(unique_ptr<NesCart> cart);
	uint8 Read(unsigned short Address) const;
	void Write(unsigned short Address, uint8 Data) const;
private:
	unique_ptr<vector<uint8>> m_cpuRam;
	unique_ptr<vector<uint8>> m_ppuRam;
	unique_ptr<NesCart> m_cart;
    uint m_memorySize = 0x0800;
};
