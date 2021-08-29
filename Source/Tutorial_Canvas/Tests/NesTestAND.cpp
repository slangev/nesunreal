// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestAnd, "Nes.AND",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestAnd)

void FNesTestAnd::Define()
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

	Describe("FNesTestAndAbsolute", [this]()
	{
		It("A = 0x55 P = 64", [this]()
		{
			cart->Write(0, 0x2D);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0xAA);
			mmu->AttachCart(move(cart));
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
			cart->Write(0, 0x31);
			cart->Write(1, 0x33);
			mmu->AttachCart(move(cart));
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0xAA);
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
			cart->Write(0, 0x39);
			cart->Write(1, 0xFF);
			cart->Write(2, 0xFF);
			mmu->Write(0x0033, 0xAA);
			mmu->AttachCart(move(cart));
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
			cart->Write(0, 0x35);
			cart->Write(1, 0x00);
			mmu->Write(0x78, 0xAA);
			mmu->AttachCart(move(cart));
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
