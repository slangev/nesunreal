// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"
#include <vector>
#include <memory>

BEGIN_DEFINE_SPEC(FNesTestLax, "Nes.LAX",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestLax)

void FNesTestLax::Define()
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

	Describe("FNesTestLax Indirect X", [this]()
	{
		It("LAX A/X = 0x55 ", [this]()
		{
			cart->Write(0, 0xA3);
			cart->Write(1, 0x40);
			mmu->AttachCart(move(cart));
			mmu->Write(0x0580, 0x55);
			mmu->Write(0x43,0x80);
			mmu->Write(0x44,0x05);
			CPU->X = 0x03;
			CPU->A = 0x00;
			CPU->P->pSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("X"), CPU->X, 0x55);
			TestEqual(TEXT("A"), CPU->A, 0x55);
			TestEqual(TEXT("Memory at 0x0580"), mmu->Read(0x0580), 0x55);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0x65);
		});
	});
}