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
	shared_ptr<NesCart> cart;
	unique_ptr<vector<int>> oam;
	unique_ptr<vector<int>> paletteRAM;
	unique_ptr<vector<int>> nameSpaceTable;
};
