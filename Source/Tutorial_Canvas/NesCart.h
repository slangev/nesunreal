// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include <vector>
#include "NesCartController.h"
#include "NesNoMapper.h"
#include "NesHeader.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesCart, Log, All);
using namespace std;
/**
 * 
 */
class TUTORIAL_CANVAS_API NesCart
{
public:
	NesCart();
	NesCart(FString pathToRom);
	~NesCart();
	void loadRom(FString pathToRom);


	

private:
	void printRomData();
	unique_ptr<NesCartController> mbc;
	unique_ptr<NesHeader> header;
	vector<uint8> romMemory;
	vector<uint8> ramMemory;
	vector<uint8> vRomMemory;
};
