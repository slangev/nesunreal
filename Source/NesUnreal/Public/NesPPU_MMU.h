// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>

using namespace std;
class NESUNREAL_API NesPPU_MMU
{
public:
	NesPPU_MMU();
	~NesPPU_MMU();
	unique_ptr<vector<int>> oam;
	unique_ptr<vector<int>> paletteRAM;
	unique_ptr<vector<int>> nameSpaceTable;
};
