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
	RenderStaticByMatrix();
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
	dynamicCanvas->SRGB = false;
	dynamicCanvas->AddToRoot();
	dynamicCanvas->Filter = TextureFilter::TF_Nearest;
	dynamicCanvas->UpdateResource();

	// buffers initialization
	bytesPerPixel = 4; // r g b a

	RenderStaticByMatrix();
}

void UDrawingCanvas::RenderStaticByMatrix() {
	FTexture2DMipMap* MyMipMap = &dynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );

	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			if(FMath::RandRange(0,2) == 0) {
				 pixel.A = 255;
				 pixel.R = 255;
				 pixel.G = 255;
				 pixel.B = 255;
			} else {
				 pixel.A = 255;
				 pixel.R = 0;
				 pixel.G = 0;
				 pixel.B = 0;
			}
			FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
		}
	}

	dynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	dynamicCanvas->UpdateResource();
}


void UDrawingCanvas::RenderLines() {
	FTexture2DMipMap* MyMipMap = &dynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );

	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			if(X % 2 == 0) {
				 pixel.A = 255;
				 pixel.R = 255;
				 pixel.G = 255;
				 pixel.B = 255;
			} else {
				 pixel.A = 255;
				 pixel.R = 0;
				 pixel.G = 0;
				 pixel.B = 0;
			}
			FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
		}
	}

	dynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	dynamicCanvas->UpdateResource();
}