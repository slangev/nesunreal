// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesCartController.h"

/**
 * 
 */
class TUTORIAL_CANVAS_API NesNoMapper : public NesCartController
{
public:
	NesNoMapper();
	~NesNoMapper();
	uint8 Read(unsigned short addr);
	void Write(unsigned short addr, uint8 data);
};
