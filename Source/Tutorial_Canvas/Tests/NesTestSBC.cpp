// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestSbc, "Nes.SBC",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestSbc)

void FNesTestSbc::Define()
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

	Describe("FNesTestSBCAbsolute", [this]()
	{
		It("A = 0x40 P = 0x65", [this]()
		{
			cart->Write(0, 0xED);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(move(cart));
			CPU->A = 0x40;
			CPU->P->pSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0x27);
		});
	});

	Describe("FNesTestSbcIndirectY", [this]()
	{
		It("A = 0x40 P = 0x65", [this]()
		{
			cart->Write(0, 0xF1);
			cart->Write(1, 0x33);
			mmu->AttachCart(move(cart));
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0x40);
			CPU->A = 0x40;
			CPU->Y = 0x00;
			CPU->P->pSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0x27);
		});
	});
}
