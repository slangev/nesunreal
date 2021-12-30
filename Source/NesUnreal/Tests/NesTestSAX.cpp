// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestSax, "Nes.SAX",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestSax)

void FNesTestSax::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesCPUMMU>();
		ppu = make_shared<NesPPU>();
		mmu->AttachPPU(ppu);
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
		//Writing the PC of 0x8000
		cart->Write(0xFFFD, 0x80);
		cart->Write(0xFFFC, 0x00);
	});	

	Describe("FNesTestSaxIndirectX", [this]()
	{
		It("A = 0x3E, X = 0x17, P = 0xE6", [this]()
		{
			cart->Write(0x8000, 0x83);
			cart->Write(0x8001, 0x49);
			mmu->AttachCart(cart);
			mmu->Write(0x60,0x89);
			mmu->Write(0x61,0x04);
			mmu->Write(0x0489, 0x00);
			CPU->AttachMemory(mmu);

			CPU->A = 0x3E;
			CPU->X = 0x17;
			CPU->P->PSetState(0xE6);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x3E);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE6);
			TestEqual(TEXT("Memory at 0x0489"), mmu->Read(0x0489), 0x16);
		});
	});
}
