// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestBit, "Nes.BIT",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestBit)

void FNesTestBit::Define()
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

	Describe("FNesTestBitAbsolute", [this]()
	{
		It("Bit at 0x678 = 0xC0 P should equal 0xE7", [this]()
		{
			cart->Write(0x8000, 0x2C);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0xC0);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x05;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x05);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0xC0);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE7);
		});
	});

	Describe("FNesTestBitZeroPage", [this]()
	{
		It("Bit at 0x01=FF P should equal 0xE5", [this]()
		{
			cart->Write(0x8000, 0x24);
			cart->Write(0x8001, 0x01);
			mmu->Write(0x01, 0xFF);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);
			
			CPU->A = 0x44;
			CPU->P->PSetState(0x27);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 3);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x44);
			TestEqual(TEXT("Memory at 0x01"), mmu->Read(0x01), 0xFF);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});
}
