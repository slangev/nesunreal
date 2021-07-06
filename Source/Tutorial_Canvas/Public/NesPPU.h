// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesPPU, Log, All);

/**
 * 
 */
class TUTORIAL_CANVAS_API NesPPU
{
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
	UTexture2D* dynamicCanvas;
	
	NesPPU();
	NesPPU(int canvasWidth,int canvasHeight, int bytesPerPixel);
	~NesPPU();
	void PrintSomething();
	void ClearScreen();
	void RenderLine();
	void RenderLines();
	void RenderStaticByMatrix();
	UTexture2D* GetScreen() {return dynamicCanvas;};

private:
	// canvas
	int canvasWidth;
	int canvasHeight;
	int bytesPerPixel;
};
