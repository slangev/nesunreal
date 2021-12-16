// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesPPU, Log, All);

/**
 * 
 */
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
	void RenderLine();
	void RenderLines();
	void RenderStaticByMatrix();
	void RenderCornerDots();
	UTexture2D* GetScreen() {return DynamicCanvas;};

private:
	// canvas
	int CanvasWidth;
	int CanvasHeight;
	int BytesPerPixel;
};
