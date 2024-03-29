// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCartController.h"
#include <vector>
using namespace std;
/**
 * 
 */
class NESUNREAL_API NesTestCart : public NesCartController
{
public:
	NesTestCart();
	NesTestCart(vector<uint8> rom);
	~NesTestCart();
	uint8 Read(unsigned short addr);
	void Write(unsigned short addr, uint8 data);
	virtual uint8 GetMirrorMode() override;
	virtual bool GetIRQRequested() override;
	virtual void UpdateIRQCounter() override;
private:
	vector<uint8> memory;
};
