// Fill out your copyright notice in the Description page of Project Settings.

#include "NesCart.h"
#include "NesHeader.h"

DEFINE_LOG_CATEGORY(LogNesCart);

NesCart::NesCart()
{
    mbc = make_unique<NesNoMapper>();
}

NesCart::NesCart(FString pathToRom) {
    loadRom(*pathToRom);
}

void NesCart::printRomData() {
	for(uint8 s : romMemory) {
		UE_LOG(LogNesCart, Log, TEXT("%X"), s);
    }
}

void NesCart::loadRom(FString pathToRom) {
    FString projectDir = FPaths::ProjectDir();
    projectDir += pathToRom;
	UE_LOG(LogNesCart, Log, TEXT("%s"), *pathToRom);
	TArray<uint8> romData;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*projectDir))
	{
		FString str = projectDir+"File does not exist!";
		UE_LOG(LogNesCart, Warning, TEXT("%s,  file does not exist!"), *str);
	}
	bool result = FFileHelper::LoadFileToArray(romData,*projectDir);
    mbc = make_unique<NesNoMapper>();
    header = make_unique<NesHeader>();
	header->nes += (char)(romData[0]);
	header->nes += (char)(romData[1]);
	header->nes += (char)(romData[2]);
	FString layerName(header->nes.c_str());
	UE_LOG(LogNesCart, Log, TEXT("%s"), *layerName);
}

NesCart::~NesCart()
{
}
