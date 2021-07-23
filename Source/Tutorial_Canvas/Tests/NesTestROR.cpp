// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestRor, "Nes.ROR",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestRor)

void FNesTestRor::Define()
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

	Describe("FNesTestRor", [this]()
	{
		It("Ror at 0x78 = 1 should equal 0x80", [this]()
		{
			cart->Write(0, 0x66);
			cart->Write(1, 0x78);
			mmu->Write(0x78, 0x01);
			mmu->AttachCart(move(cart));
			CPU->A = 0x01;
			CPU->P->pSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("Memory at 0x78"), mmu->Read(0x78), 0x80);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestRorAbsolute", [this]()
	{
		It("A = 0x01 P = 0x65", [this]()
		{
			cart->Write(0, 0x6E);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x01);
			mmu->AttachCart(move(cart));
			CPU->A = 0x01;
			CPU->P->pSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE5);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0x80);
		});
	});
}
