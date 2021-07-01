// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "NesCartController.h"
#include "NesNoMapper.h"

/**
 * 
 */
class TUTORIAL_CANVAS_API NesCart
{
public:
	NesCart();
	NesCart(FString pathToRom);
	~NesCart();
	std::unique_ptr<NesCartController> mbc;
};
