// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCart.h"
#include <vector>
#include <memory>

using namespace std;
class NESUNREAL_API NesPPUMMU
{
public:
	NesPPUMMU(shared_ptr<NesCart> cart);
	~NesPPUMMU();
	uint8 Read(unsigned short Address) const;
	void Write(unsigned short Address, uint8 Data) const;
	shared_ptr<NesCart> cart;
	unique_ptr<vector<int>> oam;
	unique_ptr<vector<int>> paletteRAM;
	unique_ptr<vector<int>> nameSpaceTable;
};
