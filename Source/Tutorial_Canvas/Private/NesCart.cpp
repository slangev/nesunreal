// Fill out your copyright notice in the Description page of Project Settings.

#include "NesCart.h"

DEFINE_LOG_CATEGORY(LogNesCart);

NesCart::NesCart()
{
    mbc = make_unique<NesNoMapper>();
}

NesCart::NesCart(vector<uint8> rom) {
	mbc = make_unique<NesTestCart>(rom);
}

NesCart::NesCart(FString pathToRom) {
    loadRom(*pathToRom);
}

void NesCart::printRomData() {
	for(const auto &s : *romMemory) {
		UE_LOG(LogNesCart, Log, TEXT("%X"), s);
    }
}

void NesCart::Log(string msg) {
	FString Fmsg(msg.c_str());
	UE_LOG(LogNesCart, Log, TEXT("%s"), *Fmsg);
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
    header = make_unique<NesHeader>();
	romMemory = make_shared<vector<uint8>>();
	ramMemory = make_shared<vector<uint8>>();
	vRomMemory = make_shared<vector<uint8>>();
	header->nes += (char)(romData[0]);
	header->nes += (char)(romData[1]);
	header->nes += (char)(romData[2]);
	header->oneA = romData[3];
	// check nes and oneA
	if(header->nes == "NES" && header->oneA == 0x1A) {
		UE_LOG(LogNesCart, Log, TEXT("NES header found!"));
    }
	header->numberOfPROROMS = romData[4];
    int romMemorySize = 0x4000 * header->numberOfPROROMS; //16KB of banks
	for(int i = 0x10; i < romMemorySize+0x10; i++) {
		romMemory->push_back(romData[i]);
	}

	int vRomMemorySize = 0x2000 * header->numberOfCHRROMS; //8KB of banks
	for(int i = 0x10+romMemorySize; i < vRomMemorySize; i++) {
		vRomMemory->push_back(romData[i]);
	}

	header->ROMControlByteOne = romData[6];
	header->ROMControlByteTwo = romData[7];
	header->numberOfRamBanks = romData[8];
	int ramMemorySize = 0x2000 * header->numberOfRamBanks;
	header->mapper = (header->ROMControlByteTwo & 0xF0) | (header->ROMControlByteOne & 0xF0 >> 4);

	uint8 mirror = (uint8)(header->ROMControlByteOne & 0x1);
	uint8 OverRide = (uint8)(header->ROMControlByteOne & 0x4 >> 3);
	header->mirroring = (uint8)((OverRide==1) ? 2 : mirror);

	switch(header->mapper) {
		case 0:
			mbc = make_unique<NesNoMapper>(romMemory,ramMemory,vRomMemory);
			Log(to_string(romMemory->size()));
			break;
		default:
			break;
	}
}

NesCart::~NesCart()
{
}

void NesCart::Write(unsigned short address, uint8 data) {
	mbc->Write(address,data);
}

uint8 NesCart::Read(unsigned short address) {
	return mbc->Read(address);
}
