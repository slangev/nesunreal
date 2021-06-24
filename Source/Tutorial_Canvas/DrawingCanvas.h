// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "Engine/Texture2D.h"
#include "Object.h"
#include "DrawingCanvas.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TUTORIAL_CANVAS_API UDrawingCanvas : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Variables)
		UTexture2D* dynamicCanvas;

	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void InitializeCanvas(const int32 pixelsH, const int32 pixelsV);
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void Tick(const int32 pixelsH);
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void RenderStaticByMatrix();
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void RenderLines();

	UDrawingCanvas();
	~UDrawingCanvas();

private:

	// canvas
	int canvasWidth;
	int canvasHeight;
	int bytesPerPixel;
};
