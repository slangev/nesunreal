// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestRla, "Nes.RLA",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestRla)

void FNesTestRla::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesMMU>();
		CPU->AttachMemory(mmu, 0x8000); //Set PC to 0x8000
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
	});

	Describe("FNesTestRLA (Indirect,X)", [this]()
	{
		It("Rla at 0xF07E", [this]()
		{
			cart->Write(0, 0x23);
			cart->Write(1, 0x45);
			mmu->Write(0x47, 0x47);
			mmu->Write(0x48, 0x06);
			mmu->Write(0x0647,0xA5);
			mmu->AttachCart(move(cart));
			CPU->A = 0xB3;
			CPU->X = 0x02;
			CPU->P->PSetState(0xE4);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 8);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x02);
			TestEqual(TEXT("Memory at 0x0647"), mmu->Read(0x0647), 0x4A);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x65);
		});
	});
}
