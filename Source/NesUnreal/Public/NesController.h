// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class NESUNREAL_API NesController
{
public:
	enum Keys : uint8 {
			Right = 0,
			Left,
			Down,
			Up,
			Start,
			Select,
			B,
			A
		};
	NesController();
	~NesController();
	void Write(unsigned Address, uint8 Data);
	uint8 Read(unsigned Address); // return one bit at a time.
	void SetKey(uint8 Key);
	void ResetKey(uint8 Key);
	void Test();
private:

	uint8 ControllerOneLatchedState = 0;
	uint8 ControllerOneCurrentState = 0;

	uint8 ControllerTwoLatchedState = 0;
	uint8 ControllerTwoCurrentState = 0;
};
