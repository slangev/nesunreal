// Fill out your copyright notice in the Description page of Project Settings.

#include "NesCart.h"
#include "NesNoMapper.h"
#include "NesTestCart.h"

DEFINE_LOG_CATEGORY(LogNesCart);

NesCart::NesCart()
{
    Mbc = make_unique<NesNoMapper>();
}

NesCart::NesCart(vector<uint8> rom) {
	Mbc = make_unique<NesTestCart>(rom);
}

NesCart::NesCart(FString PathToRom) {
    LoadRom(*PathToRom);
}

void NesCart::PrintRomData() {
	for(const auto &s : *RomMemory) {
		UE_LOG(LogNesCart, Log, TEXT("%X"), s);
    }
}

void NesCart::Log(const string Msg) {
	const FString Fmsg(Msg.c_str());
	UE_LOG(LogNesCart, Log, TEXT("%s"), *Fmsg);
}

void NesCart::LoadRom(FString PathToRom) {
    FString projectDir = FPaths::ProjectDir();
    projectDir += PathToRom;
	UE_LOG(LogNesCart, Log, TEXT("%s"), *PathToRom);
	TArray<uint8> romData;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*projectDir))
	{
		const FString Str = projectDir+"File does not exist!";
		UE_LOG(LogNesCart, Warning, TEXT("%s,  file does not exist!"), *Str);
	}
    const bool bResult = FFileHelper::LoadFileToArray(romData,*projectDir);
	if(bResult)
	{
		UE_LOG(LogNesCart, Warning, TEXT("Found file."));
	}
    Header = make_unique<FNesHeader>();
	RomMemory = make_shared<vector<uint8>>();
	RAMMemory = make_shared<vector<uint8>>();
	VRomMemory = make_shared<vector<uint8>>();
	Header->Nes += static_cast<char>(romData[0]);
	Header->Nes += static_cast<char>(romData[1]);
	Header->Nes += static_cast<char>(romData[2]);
	Header->OneA = romData[3];
	// check nes and oneA
	if(Header->Nes == "NES" && Header->OneA == 0x1A) {
		UE_LOG(LogNesCart, Log, TEXT("NES header found!"));
    }
	Header->NumberOfProroms = romData[4];
    const int RomMemorySize = 0x4000 * Header->NumberOfProroms; //16KB of banks
	for(int i = 0x10; i < RomMemorySize+0x10; i++) {
		RomMemory->push_back(romData[i]);
	}

    const int VRomMemorySize = 0x2000 * Header->NumberOfChrroms; //8KB of banks
	for(int i = 0x10+RomMemorySize; i < VRomMemorySize; i++) {
		VRomMemory->push_back(romData[i]);
	}

	Header->RomControlByteOne = romData[6];
	Header->RomControlByteTwo = romData[7];
	Header->NumberOfRamBanks = romData[8];
	//int ramMemorySize = 0x2000 * Header->NumberOfRamBanks;
	Header->Mapper = (Header->RomControlByteTwo & 0xF0) | (Header->RomControlByteOne & 0xF0 >> 4);

    const uint8 Mirror = static_cast<uint8>(Header->RomControlByteOne & 0x1);
    const uint8 OverRide = static_cast<uint8>(Header->RomControlByteOne & 0x4 >> 3);
	Header->Mirroring = static_cast<uint8>((OverRide == 1) ? 2 : Mirror);

	switch(Header->Mapper) {
		case 0:
			Mbc = make_unique<NesNoMapper>(RomMemory,RAMMemory,VRomMemory);
			Log(to_string(RomMemory->size()));
			break;
		default:
			break;
	}
}

NesCart::~NesCart()
{
}

void NesCart::Write(unsigned short Address, uint8 Data) {
	Mbc->Write(Address,Data);
}

uint8 NesCart::Read(unsigned short Address) {
	return Mbc->Read(Address);
}
