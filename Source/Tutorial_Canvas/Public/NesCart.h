// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include <vector>
#include "NesHeader.h"
#include "NesCartController.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesCart, Log, All);
using namespace std;
/**
 * 
 */
class TUTORIAL_CANVAS_API NesCart
{
public:
	NesCart();
	explicit NesCart(vector<uint8>);
	explicit NesCart(FString PathToRom);
	~NesCart();
	uint8 Read(unsigned short Address);
	void Write(unsigned short Address, uint8 Data);

private:
	void LoadRom(FString PathToRom);
	void PrintRomData();
	static void Log(string Msg);
	unique_ptr<NesCartController> Mbc;
	unique_ptr<FNesHeader> Header;
	shared_ptr<vector<uint8>> RomMemory;
	shared_ptr<vector<uint8>> RAMMemory;
	shared_ptr<vector<uint8>> VRomMemory;
};
