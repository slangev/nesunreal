// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestAnd, "Nes.AND",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestAnd)

void FNesTestAnd::Define()
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

	Describe("FNesTestAndAbsolute", [this]()
	{
		It("A = 0x55 P = 64", [this]()
		{
			cart->Write(0x8000, 0x2D);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0xAA);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x55;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x66);
		});
	});

	Describe("FNesTestAndIndirectY", [this]()
	{
		It("A = 0x55 P = 0x64", [this]()
		{
			cart->Write(0x8000, 0x31);
			cart->Write(0x8001, 0x33);
			mmu->AttachCart(cart);
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0xAA);
			CPU->AttachMemory(mmu);

			CPU->A = 0x55;
			CPU->Y = 0x00;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x66);
		});
	});

	Describe("FNesTestAndAbsoluteY", [this]()
	{
		It("And Y at 0x0033 = 0xAA should equal 0xFF", [this]()
		{
			cart->Write(0x8000, 0x39);
			cart->Write(0x8001, 0xFF);
			cart->Write(0x8002, 0xFF);
			mmu->Write(0x0033, 0xAA);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x55;
			CPU->Y = 0x34;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x66);
		});
	});

	Describe("FNesTestAndZeroPageX", [this]()
	{
		It("A = 0x00 P = E4", [this]()
		{
			cart->Write(0x8000, 0x35);
			cart->Write(0x8001, 0x00);
			mmu->Write(0x78, 0xAA);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x55;
			CPU->X = 0x78;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x66);
		});
	});
}
