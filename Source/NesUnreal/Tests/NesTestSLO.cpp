// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestSlo, "Nes.SLO",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestSlo)

void FNesTestSlo::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesMMU>();
		ppu = make_shared<NesPPU>();
		mmu->AttachPPU(ppu);
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
		//Writing the PC of 0x8000
		cart->Write(0xFFFD, 0x80);
		cart->Write(0xFFFC, 0x00);
	});

	Describe("FNesTestSLO (Indirect,X)", [this]()
	{
		It("Slo at 0xEE0E", [this]()
		{
			cart->Write(0x8000, 0x03);
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
			TestEqual(TEXT("A"), CPU->A, 0xFB);
			TestEqual(TEXT("Memory at 0x0647"), mmu->Read(0x0647), 0x4A);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestSLO Zero Page X", [this]()
	{
		It("Slo at 0xEF68", [this]()
		{
			cart->Write(0x8000, 0x17);
			cart->Write(0x8001, 0x48);
			mmu->Write(0x47, 0xA5);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0xB3;
			CPU->X = 0xFF;
			CPU->P->PSetState(0xE4);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0xFB);
			TestEqual(TEXT("Memory at 0x0647"), mmu->Read(0x47), 0x4A);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});
}
