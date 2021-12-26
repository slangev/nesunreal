// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include "NesCartController.h"
using namespace std;
/**
 * 
 */
class NESUNREAL_API NesNoMapper : public NesCartController
{
public:
	NesNoMapper();
	NesNoMapper(shared_ptr<vector<uint8>> PRGRomMemory, shared_ptr<vector<uint8>> PRGRamMemory, shared_ptr<vector<uint8>> ChrRomMemory, shared_ptr<vector<uint8>> ChrRamMemory);
	virtual ~NesNoMapper() override;
	virtual uint8 Read(unsigned short address) override;
	virtual void Write(unsigned short address, uint8 data) override;
private:
	shared_ptr<vector<uint8>> PRGRomMemory;
    shared_ptr<vector<uint8>> PRGRamMemory;
    shared_ptr<vector<uint8>> ChrRomMemory;
	shared_ptr<vector<uint8>> ChrRamMemory;
};
