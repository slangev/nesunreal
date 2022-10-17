// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesCartController
{
public:
	virtual ~NesCartController();
	virtual uint8 Read(unsigned short Address) = 0;
	virtual void Write(unsigned short Address, uint8 Data) = 0;
	virtual uint8 GetMirrorMode() = 0;
	virtual bool GetIRQRequested() = 0;
	virtual void UpdateIRQCounter() = 0;
};
