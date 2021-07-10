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
uint m_memorySize = 0x4000;
vector<uint8> rom;

void Setup() {
	rom(0,m_memorySize);
	cpu = make_unique<NesCPU>();
	mmu = make_shared<NesMMU>();
    cpu->AttachMemory(mmu,0x8000); //Set PC to 0x8000*/
}

bool FMyAwesomeTestLD::RunTest(const FString& Parameters)
{
	Setup();
	unique_ptr<NesCart> cart = make_unique<NesCart>(rom);
	cart->Write(0,0xA2);
	cart->Write(1,0x00);
	mmu->AttachCart(move(cart));
	cpu->X = 0x1; 
	uint8 cycle = cpu->Tick();
	TestEqual(TEXT("`Cycle needs to be 2"), cycle, 2);
	//Assert.AreEqual(0x8002, nesCPU->PC);
	//Assert.AreEqual(0x0,nesCPU->X);
	//Assert.AreEqual(0x36,nesCPU->pStateWithBFlag());
	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
