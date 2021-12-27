// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestIsb, "Nes.ISB",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
shared_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestIsb)

void FNesTestIsb::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesMMU>();
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
		//Writing the PC of 0x8000
		cart->Write(0xFFFD, 0x80);
		cart->Write(0xFFFC, 0x00);
	});

	Describe("FNesTestISB (Indirect,X)", [this]()
	{
		It("Isb at 0xEB9E", [this]()
		{
			cart->Write(0x8000, 0xE3);
			cart->Write(0x8001, 0x45);
			mmu->Write(0x78, 0x00);
			mmu->Write(0x47, 0x47);
			mmu->Write(0x48, 0x06);
			mmu->Write(0x0647,0xEB);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x40;
			CPU->X = 0x02;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 8);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x53);
			TestEqual(TEXT("Memory at 0x0647"), mmu->Read(0x0647), 0xEC);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});
}
