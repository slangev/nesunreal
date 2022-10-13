// Fill out your copyright notice in the Description page of Project Settings.

#include "NesCart.h"
#include "NesNoMapper.h"
#include "NesTestCart.h"
#include "NesCNROM.h"
#include "NesMMC1.h"
#include "NesMMC3.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesCart,Log,All)

NesCart::NesCart()
{
    Mbc = make_unique<NesNoMapper>();
}

NesCart::NesCart(vector<uint8> rom) 
{
	Mbc = make_unique<NesTestCart>(rom);
}

NesCart::NesCart(FString PathToRom) 
{
    LoadRom(*PathToRom);
}

void NesCart::PrintRomData() 
{
	for(const auto &s : *PrgRomMemory) 
	{
		UE_LOG(LogNesCart, Log, TEXT("%X"), s);
    }
}

void NesCart::Log(const string Msg) 
{
	const FString Fmsg(Msg.c_str());
	UE_LOG(LogNesCart, Log, TEXT("%s"), *Fmsg);
}

void NesCart::LoadRom(FString PathToRom) 
{
    projectDir = PathToRom;
	UE_LOG(LogNesCart, Log, TEXT("%s"), *projectDir);
	TArray<uint8> romData;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*projectDir))
	{
		const FString Str = projectDir+"File does not exist!";
		UE_LOG(LogNesCart, Warning, TEXT("%s, file does not exist!"), *Str);
	}
    const bool bResult = FFileHelper::LoadFileToArray(romData,*projectDir);
	if(bResult)
	{
		UE_LOG(LogNesCart, Log, TEXT("Found file."));
	}
    Header = make_unique<FNesHeader>();
	PrgRomMemory = make_shared<vector<uint8>>();
	ChrRomMemory = make_shared<vector<uint8>>();
	Header->Nes += static_cast<char>(romData[0]);
	Header->Nes += static_cast<char>(romData[1]);
	Header->Nes += static_cast<char>(romData[2]);
	Header->OneA = romData[3];
	// check nes and oneA
	if(Header->Nes == "NES" && Header->OneA == 0x1A) 
	{
		UE_LOG(LogNesCart, Log, TEXT("NES header found!"));
    }
	Header->NumberOfPrgRoms = romData[4];
	Header->NumberOfChrRoms = romData[5];
	
    const int RomMemorySize = 0x4000 * Header->NumberOfPrgRoms; //16KB of banks
	for(int i = 0x10; i < RomMemorySize+0x10; i++) 
	{
		PrgRomMemory->push_back(romData[i]);
	}

    const int VRomMemorySize = 0x2000 * Header->NumberOfChrRoms; //8KB of banks
	for(int i = 0; i < VRomMemorySize; i++) 
	{
		ChrRomMemory->push_back(romData[i + RomMemorySize + 0x10]);
	}

	//NumberOfChrRoms/romData[5]: Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	if(Header->NumberOfChrRoms == 0) 
	{
		ChrRamMemory = make_shared<vector<uint8>>(0x2000);
		UE_LOG(LogNesCart, Warning, TEXT("ChrRamMemory in Use!"));
	}

	Header->RomControlByteOne = romData[6];
	Header->RomControlByteTwo = romData[7];
	Header->NumberOfRamBanks = romData[8];
	if(Header->NumberOfRamBanks == 0) 
	{
		PrgRamMemory = make_shared<vector<uint8>>(0x2000);
	} 
	else 
	{
		uint RAMMemorySize = 0x2000 * Header->NumberOfRamBanks;
		PrgRamMemory = make_shared<vector<uint8>>(RAMMemorySize);
	}
	Header->Mapper = (Header->RomControlByteTwo & 0xF0) | (Header->RomControlByteOne & 0xF0) >> 4;
    const uint8 Mirror = static_cast<uint8>(Header->RomControlByteOne & 0x1);
	UE_LOG(LogNesCart,Warning,TEXT("Mirror: %d"),Header->Mirroring);
    uint8 OverRide = static_cast<uint8>(Header->RomControlByteOne & 0x8 >> 3);
	OverRide = 0;
	Header->Mirroring = static_cast<uint8>((OverRide == 1) ? 2 : Mirror);
	// UE_LOG(LogNesCart,Warning,TEXT("Mirror2: %d"),Header->Mirroring);
	this->bBatteryBacked = (Header->RomControlByteOne & 0x2) == 0x2;
	UE_LOG(LogNesCart,Warning,TEXT("bBatteryBacked %s"),(this->bBatteryBacked) ? TEXT("True") : TEXT("False"));
	UE_LOG(LogNesCart,Warning,TEXT("MAPPER: %d"),Header->Mapper);

	//https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/Containers/FString/FindLastChar/
	int index = 0;
	bool bHasFound = projectDir.FindLastChar('/',index);
	if(bHasFound) 
	{
		FString title = projectDir.Mid(index+1);
		UE_LOG(LogNesCart,Warning,TEXT("Title: %s"),*title);
		Header->GameTitle=title;
	}
	Load();
	switch(Header->Mapper) 
	{
		case 0:
			Mbc = make_unique<NesNoMapper>(PrgRomMemory,PrgRamMemory,ChrRomMemory, ChrRamMemory);
			break;
		case 1:
			Mbc = make_unique<NesMMC1>(PrgRomMemory,PrgRamMemory,ChrRomMemory, ChrRamMemory, this->bBatteryBacked);
			break;
		case 3:
			Mbc = make_unique<NesCNROM>(PrgRomMemory,PrgRamMemory,ChrRomMemory, ChrRamMemory);
			break;
		case 4:
			Mbc = make_unique<NesMMC3>(PrgRomMemory,PrgRamMemory,ChrRomMemory, ChrRamMemory, this->bBatteryBacked);
		default:
			break;
	}
}

NesCart::~NesCart()
{
	Save();
}

bool NesCart::Save()
{
	if(this->bBatteryBacked)
	{
		TArray<uint8> ramData;
		for(int i = 0; i < this->PrgRamMemory->size(); i++) 
		{
			ramData.Add(this->PrgRamMemory->at(i));
		}
		FString SaveFile = projectDir + ".save";
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveFile))
		{
			UE_LOG(LogNesCart, Warning, TEXT("%s, file does not exist! Creating save file"), *SaveFile);
		}
		bool bResult = FFileHelper::SaveArrayToFile(ramData,*SaveFile);
		return bResult;
	}
	return false;
}

bool NesCart::Load()
{
	if(this->bBatteryBacked)
	{
		FString SaveFile = projectDir + ".save";
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveFile))
		{
			UE_LOG(LogNesCart, Warning, TEXT("%s, file does not exist!"), *SaveFile);
		}
		else 
		{
			TArray<uint8> ramData;
			const bool bResult = FFileHelper::LoadFileToArray(ramData,*SaveFile);
			if(bResult)
			{
				UE_LOG(LogNesCart, Warning, TEXT("Successfully found save file"));
				for(uint32 i = 0; i < ramData.Num(); i++) 
				{
					this->PrgRamMemory->at(i) = ramData[i];
				}
			}
			return bResult;	
		}
	}
	return false;
}

void NesCart::Write(unsigned short Address, uint8 Data) 
{
	Mbc->Write(Address,Data);
}

uint8 NesCart::Read(unsigned short Address) 
{
	return Mbc->Read(Address);
}

uint8 NesCart::GetMirrorMode() 
{
	// Return mirror mode
	uint8 mode = Mbc->GetMirrorMode();
	return mode;
}
