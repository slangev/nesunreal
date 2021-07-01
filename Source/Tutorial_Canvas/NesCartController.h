// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TUTORIAL_CANVAS_API NesCartController
{
public:
	virtual ~NesCartController();
	virtual uint8 Read(unsigned short addr) = 0;
	virtual void Write(unsigned short addr, uint8 data) = 0;
};
