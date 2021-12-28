// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NesPixel.h"

/**
 * 
 */
using namespace std;
class NESUNREAL_API NesPPU
{
public:
	UTexture2D* DynamicCanvas;
	
	NesPPU();
	NesPPU(int CanvasWidth,int CanvasHeight, int BytesPerPixel);
	~NesPPU();
	void PrintStats(int32 x, int32 y);
	void ClearScreen();
	void RenderStaticByMatrix();
	void RenderCornerDots();
	void RenderScreen();
	void SetNMIInterrupt(bool request);
	bool GetNMIInterrupt();
	void Step(uint cycle);
	UTexture2D* GetScreen() {return DynamicCanvas;};

private:
	// canvas
	int CanvasWidth;
	int CanvasHeight;
	int BytesPerPixel;
	bool NMI = false;

	// Frame Buffer
	unique_ptr<vector<unique_ptr<vector<NesPixel>>>> VideoMemory;
};
