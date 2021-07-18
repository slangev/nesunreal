// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestEOR, "Nes.EOR",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestEOR)

void FNesTestEOR::Define()
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

	Describe("FNesTestEOR", [this]()
	{
		It("A = 0xF5 P = E4", [this]()
		{
			cart->Write(0, 0x45);
			cart->Write(1, 0x78);
			mmu->Write(0x78, 0xAA);
			mmu->AttachCart(move(cart));
			CPU->A = 0x5F;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle needs to be 3"), Cycle, 3);
			TestEqual(TEXT("PC needs to be 0x8002"), CPU->PC, 0x8002);
			TestEqual(TEXT("A needs to be 0xF5"), CPU->A, 0xF5);
			TestEqual(TEXT("P needs to be 0xE4"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});
}
