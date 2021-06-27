// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPPU.h"
DEFINE_LOG_CATEGORY(LogNesPPU);

NesPPU::NesPPU()
{
    //dynamic texture initialization
	canvasWidth = 256;
	canvasHeight = 240;

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
}

NesPPU::NesPPU(int canvasWidth, int canvasHeight, int bytesPerPixel) {
    this->canvasWidth = canvasWidth;
    this->canvasHeight = canvasHeight;
    this->bytesPerPixel = bytesPerPixel;
	dynamicCanvas = UTexture2D::CreateTransient(canvasWidth, canvasHeight);
#if WITH_EDITORONLY_DATA
	dynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	dynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicCanvas->SRGB = true;
	dynamicCanvas->AddToRoot();
	dynamicCanvas->Filter = TextureFilter::TF_Nearest;
	dynamicCanvas->UpdateResource();
	ClearScreen();
}

NesPPU::~NesPPU() 
{
}

void NesPPU::PrintSomething() {
    UE_LOG(LogTemp, Warning, TEXT("Hello World!"));
}

void NesPPU::RenderStaticByMatrix() {
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

void NesPPU::RenderLine() {
	FTexture2DMipMap* MyMipMap = &dynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );

	for(int32 X = 0; X < 10; X = X + 2) 
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			pixel.A = 255;
			pixel.R = 255;
			pixel.G = 0;
			pixel.B = 0;
			FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
		}
	}

	dynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	dynamicCanvas->UpdateResource();
}


void NesPPU::RenderLines() {
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

void NesPPU::ClearScreen() {
	FTexture2DMipMap* MyMipMap = &dynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );

	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			pixel.A = 255;
			pixel.R = 255;
			pixel.G = 255;
			pixel.B = 255;
			FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
		}
	}

	dynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	dynamicCanvas->UpdateResource();
}
