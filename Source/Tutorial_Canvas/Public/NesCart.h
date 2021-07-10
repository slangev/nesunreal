// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include <vector>
#include "NesHeader.h"
#include "NesCartController.h"
#include "NesNoMapper.h"
#include "NesTestCart.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesCart, Log, All);
using namespace std;
/**
 * 
 */
class TUTORIAL_CANVAS_API NesCart
{
public:
	NesCart();
	NesCart(vector<uint8>);
	NesCart(FString pathToRom);
	~NesCart();
	uint8 Read(unsigned short address);
	void Write(unsigned short address, uint8 data);

private:
	void loadRom(FString pathToRom);
	void printRomData();
	void Log(string msg);
	unique_ptr<NesCartController> mbc;
	unique_ptr<NesHeader> header;
	shared_ptr<vector<uint8>> romMemory;
	shared_ptr<vector<uint8>> ramMemory;
	shared_ptr<vector<uint8>> vRomMemory;
};
