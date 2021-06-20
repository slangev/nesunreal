// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingCanvas.h"

UDrawingCanvas::UDrawingCanvas()
{
}

UDrawingCanvas::~UDrawingCanvas()
{
}

void UDrawingCanvas::Tick(const int32 pixelsH) {
	//UE_LOG(LogTemp, Warning, TEXT("I just started running"));
	RenderStatic();
}

void UDrawingCanvas::InitializeCanvas(const int32 pixelsH, const int32 pixelsV)
{
	//dynamic texture initialization
	canvasWidth = pixelsH;
	canvasHeight = pixelsV;

	dynamicCanvas = UTexture2D::CreateTransient(canvasWidth, canvasHeight);
#if WITH_EDITORONLY_DATA
	dynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	dynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicCanvas->SRGB = 1;
	dynamicCanvas->AddToRoot();
	dynamicCanvas->Filter = TextureFilter::TF_Nearest;
	dynamicCanvas->UpdateResource();

	echoUpdateTextureRegion = std::unique_ptr<FUpdateTextureRegion2D>(new FUpdateTextureRegion2D(0, 0, 0, 0, canvasWidth, canvasHeight));


	// buffers initialization
	bytesPerPixel = 4; // r g b a
	bufferPitch = canvasWidth * bytesPerPixel;
	bufferSize = canvasWidth * canvasHeight * bytesPerPixel;
	canvasPixelData = std::unique_ptr<uint8[]>(new uint8[bufferSize]);

	ClearCanvas();
}

void UDrawingCanvas::ClearCanvas()
{
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int i = 0; i < canvasWidth * canvasHeight; ++i)
	{
		setPixelColor(canvasPixelPtr, 255, 255, 255, 255); //white
		canvasPixelPtr += bytesPerPixel;
	}
	UpdateCanvas();
}

void UDrawingCanvas::RenderLine() {
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int i = 0; i < canvasWidth; ++i)
	{
		if(i % 2 == 0) {
			setPixelColor(canvasPixelPtr, 255, 255, 255, 255); //white
		} else {
			setPixelColor(canvasPixelPtr, 0, 0, 0, 255); //black
		}
		canvasPixelPtr += bytesPerPixel;
	}
	UpdateCanvas();
}

void UDrawingCanvas::RenderStatic() {
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int i = 0; i < canvasWidth * canvasHeight; ++i)
	{
		if(FMath::RandRange(0,2) == 0) {
			setPixelColor(canvasPixelPtr, 255, 255, 255, 255); //white
		} else {
			setPixelColor(canvasPixelPtr, 0, 0, 0, 255); //black
		}
		canvasPixelPtr += bytesPerPixel;
	}
	UpdateCanvas();
}

void UDrawingCanvas::RenderDot() {
	uint8* canvasPixelPtr = canvasPixelData.get();
	for (int i = 0; i < 1; ++i)
	{
		if(i % 2 == 0) {
			setPixelColor(canvasPixelPtr, 255, 0, 0, 255); //red
		} else {
			setPixelColor(canvasPixelPtr, 0, 255, 0, 255); //green
		}
		canvasPixelPtr += bytesPerPixel;
	}
	UpdateCanvas();
}

void UDrawingCanvas::UpdateCanvas()
{
	if (echoUpdateTextureRegion)
	{
		dynamicCanvas->UpdateTextureRegions((int32)0, (uint32)1, echoUpdateTextureRegion.get(), (uint32)bufferPitch, (uint32)bytesPerPixel, canvasPixelData.get());
	}
}

void UDrawingCanvas::setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	*pointer = blue; //b
	*(pointer + 1) = green; //g
	*(pointer + 2) = red; //r
	*(pointer + 3) = alpha; //a
}