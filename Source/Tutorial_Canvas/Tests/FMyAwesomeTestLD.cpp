// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"
#include <vector>
#include <memory>

#if WITH_DEV_AUTOMATION_TESTS 

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyAwesomeTestLD, "Nes.LD", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

unique_ptr<NesCPU> cpu;
shared_ptr<NesMMU> mmu;
unique_ptr<vector<uint8>> rom;
uint m_memorySize = 0x4000;

void Setup() {
	rom = make_unique<vector<uint8>>(m_memorySize,0);
	cpu = make_unique<NesCPU>();
	mmu = make_shared<NesMMU>();
	//nesCart = new NesCart(rom);
    cpu->AttachMemory(mmu,0x8000); //Set PC to 0x8000*/
    //mmu->AttachCart(make_unique<NesCart>(pathToRom));
}

bool FMyAwesomeTestLD::RunTest(const FString& Parameters)
{
	Setup();
	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
