// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API NesPixel
{
public:
	NesPixel();
	~NesPixel();
	FColor pixel;
	bool bIsTransparent = false;
};
