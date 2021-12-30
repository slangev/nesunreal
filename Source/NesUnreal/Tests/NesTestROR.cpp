// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestRor, "Nes.ROR",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestRor)

void FNesTestRor::Define()
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

	Describe("FNesTestRor", [this]()
	{
		It("Ror at 0x78 = 1 should equal 0x80", [this]()
		{
			cart->Write(0x8000, 0x66);
			cart->Write(0x8001, 0x78);
			mmu->Write(0x78, 0x01);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x01;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("Memory at 0x78"), mmu->Read(0x78), 0x80);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestRorAbsolute", [this]()
	{
		It("A = 0x01 P = 0x65", [this]()
		{
			cart->Write(0x8000, 0x6E);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x01);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x01;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0x80);
		});
	});
}
