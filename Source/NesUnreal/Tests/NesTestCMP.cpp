// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestCmp, "Nes.CMP",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestCmp)

void FNesTestCmp::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesMMU>();
		CPU->AttachMemory(mmu); //Set PC to 0x8000
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
	});

	Describe("FNesTestCMPAbsolute", [this]()
	{
		It("A = 0x5F P = 64", [this]()
		{
			cart->Write(0, 0xCD);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(move(cart));
			CPU->A = 0x40;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestCPXAbsolute", [this]()
	{
		It("X = 0x5F P = 64", [this]()
		{
			cart->Write(0, 0xEC);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(move(cart));
			CPU->X = 0x40;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("X"), CPU->X, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});
	
	Describe("FNesTestCPYAbsolute", [this]()
	{
		It("Y = 0x5F P = 64", [this]()
		{
			cart->Write(0, 0xCC);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(move(cart));
			CPU->Y = 0x40;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("X"), CPU->Y, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestCMPIndirectY", [this]()
	{
		It("A = 0x40 P = 0x65", [this]()
		{
			cart->Write(0, 0xD1);
			cart->Write(1, 0x33);
			mmu->AttachCart(move(cart));
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0x40);
			CPU->A = 0x40;
			CPU->Y = 0x00;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestCMPAbsoluteY", [this]()
	{
		It("CMP Y at 0x0033 = 0x40 should equal 0x40", [this]()
		{
			cart->Write(0, 0xD9);
			cart->Write(1, 0xFF);
			cart->Write(2, 0xFF);
			mmu->Write(0x0033, 0x40);
			mmu->AttachCart(move(cart));
			CPU->A = 0x40;
			CPU->Y = 0x34;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestCmpZeroPageX", [this]()
	{
		It("A = 0x40 P = 65", [this]()
		{
			cart->Write(0, 0xD5);
			cart->Write(1, 0x00);
			mmu->Write(0x78, 0x40);
			mmu->AttachCart(move(cart));
			CPU->A = 0x40;
			CPU->X = 0x78;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});
}
