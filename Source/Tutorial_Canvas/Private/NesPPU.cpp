// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPPU.h"

DEFINE_LOG_CATEGORY(LogNesPPU);

NesPPU::NesPPU()
{
    //dynamic texture initialization
	CanvasWidth = 256;
	CanvasHeight = 240;

	DynamicCanvas = UTexture2D::CreateTransient(CanvasWidth, CanvasHeight);
	DynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicCanvas->SRGB = false;
	DynamicCanvas->AddToRoot();
	DynamicCanvas->Filter = TextureFilter::TF_Nearest;
	DynamicCanvas->UpdateResource();

	// buffers initialization
	BytesPerPixel = 4; // r g b a
	ClearScreen();
}

NesPPU::NesPPU(int CanvasWidth, int CanvasHeight, int BytesPerPixel) {
    this->CanvasWidth = CanvasWidth;
    this->CanvasHeight = CanvasHeight;
    this->BytesPerPixel = BytesPerPixel;
	DynamicCanvas = UTexture2D::CreateTransient(CanvasWidth, CanvasHeight);
	DynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicCanvas->SRGB = true;
	DynamicCanvas->AddToRoot();
	DynamicCanvas->Filter = TextureFilter::TF_Nearest;
	DynamicCanvas->UpdateResource();
	ClearScreen();
}

NesPPU::~NesPPU() 
{
}

void NesPPU::PrintStats(int32 x, int32 y) {
	FString xStr = FString::FromInt(x);
	FString yStr = FString::FromInt(y);
    UE_LOG(LogNesPPU, Warning, TEXT("%s %s"), *xStr, *yStr);
}

void NesPPU::RenderCornerDots() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );
	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			//Upper left red dot
			if(X == 0 && Y == 0) {
				pixel.A = 255;
				pixel.R = 255;
				pixel.G = 0;
				pixel.B = 0;
				FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
			}
			//Bottom left green dot
			if(X == 0 && Y == 239) {
				pixel.A = 255;
				pixel.R = 0;
				pixel.G = 255;
				pixel.B = 0;
				FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
			}
			//Upper right blue dot
			if(X == 255 && Y == 0) {
				pixel.A = 255;
				pixel.R = 0;
				pixel.G = 0;
				pixel.B = 255;
				FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
			}

			//Lower right black dot
			if(X == 255 && Y == 239) {
				pixel.A = 255;
				pixel.R = 0;
				pixel.G = 0;
				pixel.B = 0;
				FormatedImageData[Y * MyMipMap->SizeX + X] = pixel;
			}
		}
	}

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}

void NesPPU::RenderStaticByMatrix() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
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

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}

void NesPPU::RenderLine() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
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

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}


void NesPPU::RenderLines() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
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

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}

void NesPPU::ClearScreen() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
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

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}
