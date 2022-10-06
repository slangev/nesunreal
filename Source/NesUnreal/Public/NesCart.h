// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include <vector>
#include "NesHeader.h"
#include "NesCartController.h"

using namespace std;
/**
 * 
 */
class NESUNREAL_API NesCart
{
public:
	NesCart();
	explicit NesCart(vector<uint8>);
	explicit NesCart(FString PathToRom);
	~NesCart();
	uint8 Read(unsigned short Address);
	void Write(unsigned short Address, uint8 Data);
	unique_ptr<FNesHeader> Header;
	uint8 GetMirrorMode();

private:
	bool Save();
	bool Load();
	void LoadRom(FString PathToRom);
	void PrintRomData();
	static void Log(string Msg);
	bool bBatteryBacked = false;
	FString projectDir = "";
	unique_ptr<NesCartController> Mbc;
	shared_ptr<vector<uint8>> PrgRomMemory;
	shared_ptr<vector<uint8>> PrgRamMemory;
	shared_ptr<vector<uint8>> ChrRomMemory;
	shared_ptr<vector<uint8>> ChrRamMemory;
};
