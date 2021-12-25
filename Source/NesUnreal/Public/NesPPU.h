// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NesPixel.h"
#include "Engine/Texture2D.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesPPU, Log, All);

/**
 * 
 */
using namespace std;
class NESUNREAL_API NesPPU
{
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
	UTexture2D* DynamicCanvas;
	
	NesPPU();
	NesPPU(int CanvasWidth,int CanvasHeight, int BytesPerPixel);
	~NesPPU();
	void PrintStats(int32 x, int32 y);
	void ClearScreen();
	void RenderStaticByMatrix();
	void RenderCornerDots();
	void RenderScreen();
	UTexture2D* GetScreen() {return DynamicCanvas;};

private:
	// canvas
	int CanvasWidth;
	int CanvasHeight;
	int BytesPerPixel;

	// Frame Buffer
	unique_ptr<vector<unique_ptr<vector<NesPixel>>>> videoMemory;
};
