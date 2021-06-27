// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS 

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyAwesomeTestST, "Nes.ST", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FMyAwesomeTestST::RunTest(const FString& Parameters)
{
	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
