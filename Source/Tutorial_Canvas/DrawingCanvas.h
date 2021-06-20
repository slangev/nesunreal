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
		void UpdateCanvas();
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void ClearCanvas();
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void Tick(const int32 pixelsH);
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void RenderLine();
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void RenderDot();
	UFUNCTION(BlueprintCallable, Category = DrawingTools)
		void RenderStatic();

	UDrawingCanvas();
	~UDrawingCanvas();

private:

	// canvas
	std::unique_ptr<uint8[]> canvasPixelData;
	int canvasWidth;
	int canvasHeight;
	int bytesPerPixel;
	int bufferPitch;
	int bufferSize;

	std::unique_ptr<FUpdateTextureRegion2D> echoUpdateTextureRegion;
	void setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha);
};
