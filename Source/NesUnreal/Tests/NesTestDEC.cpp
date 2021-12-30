// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestDec, "Nes.DEC",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestDec)

void FNesTestDec::Define()
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

	Describe("FNesTestDec", [this]()
	{
		It("Inc at 0x78 = 0xFF should equal 0x00", [this]()
		{
			cart->Write(0x8000, 0xC6);
			cart->Write(0x8001, 0x78);
			mmu->Write(0x78, 0x00);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x00;
			CPU->P->PSetState(0xE7);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("Memory at 0x78"), mmu->Read(0x78), 0xFF);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});
	
	Describe("FNesTestDecAbsolute", [this]()
	{
		It("A = 0x00 P = 0xE7", [this]()
		{
			cart->Write(0x8000, 0xCE);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x00);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);
			
			CPU->A = 0x00;
			CPU->P->PSetState(0xE7);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0xFF);
		});
	});
}
