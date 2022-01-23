// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NESUNREAL_API FNesSoundChannel
{
public:
	FNesSoundChannel();
	virtual ~FNesSoundChannel();
	virtual void Tick()=0;
	virtual void HalfFrameTick()=0;
	virtual void QuarterFrameTick()=0;
	virtual void WriteData(int Address, int Data)=0;
	virtual void Enabled(bool bEnabled)=0;
	virtual int GetOutputVol()=0;
	virtual bool LengthAboveZero()=0;
};
