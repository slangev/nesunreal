// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesCPUMMU.h"
#include "NesPPU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestRla, "Nes.RLA",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestRla)

void FNesTestRla::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesCPUMMU>();
		ppu = make_shared<NesPPU>();
		mmu->AttachPpu(ppu);
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
		//Writing the PC of 0x8000
		cart->Write(0xFFFD, 0x80);
		cart->Write(0xFFFC, 0x00);
	});

	Describe("FNesTestRLA (Indirect,X)", [this]()
	{
		It("Rla at 0xF07E", [this]()
		{
			cart->Write(0x8000, 0x23);
			cart->Write(0x8001, 0x45);
			mmu->Write(0x47, 0x47);
			mmu->Write(0x48, 0x06);
			mmu->Write(0x0647,0xA5);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

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
