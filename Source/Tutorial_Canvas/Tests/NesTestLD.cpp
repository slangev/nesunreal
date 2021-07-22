// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"
#include <vector>
#include <memory>


unique_ptr<NesCPU> cpu;
shared_ptr<NesMMU> mmu;
uint m_memorySize = 0x4000;
vector<uint8> rom;

void Setup() {
	cpu = make_unique<NesCPU>();
	mmu = make_shared<NesMMU>();
    cpu->AttachMemory(mmu,0x8000); //Set PC to 0x8000
	rom.clear();
}

#if WITH_DEV_AUTOMATION_TESTS 

IMPLEMENT_SIMPLE_AUTOMATION_TEST(NesImmediateLDXSetZFlag, "Nes.LD.NesImmediateLDXSetZFlag", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool NesImmediateLDXSetZFlag::RunTest(const FString& Parameters)
{
	Setup();
    rom.resize(0x8000,0);
	unique_ptr<NesCart> cart = make_unique<NesCart>(rom);
	cart->Write(0,0xA2);
	cart->Write(1,0x00);
	mmu->AttachCart(move(cart));
	cpu->X = 0x1; 
	TestEqual(TEXT("PC needs to be 0x8000"), cpu->PC, 0x8000);
	const uint8 Cycle = cpu->Tick();
	TestEqual(TEXT("Cycle needs to be 2"), Cycle, 2);
	TestEqual(TEXT("X needs to be 0"), cpu->X, 0);
	TestEqual(TEXT("PC needs to be 0x8002"), cpu->PC, 0x8002);
	TestEqual(TEXT("P needs to be 0x36"), cpu->P->pStateWithBFlag(), 0x36);
	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS

#if WITH_DEV_AUTOMATION_TESTS 

IMPLEMENT_SIMPLE_AUTOMATION_TEST(NesImmediateLDYSetZFlag, "Nes.LD.NesImmediateLDYSetZFlag", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool NesImmediateLDYSetZFlag::RunTest(const FString& Parameters)
{
	Setup();
    rom.resize(0x8000,0);
	unique_ptr<NesCart> cart = make_unique<NesCart>(rom);
	cart->Write(0,0xA0);
	cart->Write(1,0x00);
	mmu->AttachCart(move(cart));
	cpu->Y = 0x1; 
	uint8 cycle = cpu->Tick();
	TestEqual(TEXT("`Cycle needs to be 2"), cycle, 2);
	TestEqual(TEXT("`Y needs to be 0"), cpu->X, 0);
	TestEqual(TEXT("`PC needs to be 0x8002"), cpu->PC, 0x8002);
	TestEqual(TEXT("`P needs to be 0x36"), cpu->P->pStateWithBFlag(), 0x36);
	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FNesTestLd, "Nes.LD",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestLd)

void FNesTestLd::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<NesCPU>();
		mmu = make_shared<NesMMU>();
		CPU->AttachMemory(mmu, 0x8000); //Set PC to 0x8000
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
	});

	Describe("FNesTestLd", [this]()
	{
		It("LDY at 0x0678 = 0x55 should equal 0x55", [this]()
		{
			cart->Write(0, 0xAC);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x55);
			mmu->AttachCart(move(cart));
			CPU->Y = 0x00;
			CPU->P->pSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("Y"), CPU->Y, 0x55);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0x55);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0x65);
		});
	});
}