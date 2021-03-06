// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestCmp, "Nes.CMP",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestCmp)

void FNesTestCmp::Define()
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

	Describe("FNesTestCMPAbsolute", [this]()
	{
		It("A = 0x5F P = 64", [this]()
		{
			cart->Write(0x8000, 0xCD);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);
			
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
			cart->Write(0x8000, 0xEC);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

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
			cart->Write(0x8000, 0xCC);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x40);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

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
			cart->Write(0x8000, 0xD1);
			cart->Write(0x8001, 0x33);
			mmu->AttachCart(cart);
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0x40);
			CPU->AttachMemory(mmu);

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
			cart->Write(0x8000, 0xD9);
			cart->Write(0x8001, 0xFF);
			cart->Write(0x8002, 0xFF);
			mmu->Write(0x0033, 0x40);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

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
			cart->Write(0x8000, 0xD5);
			cart->Write(0x8001, 0x00);
			mmu->Write(0x78, 0x40);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);
			
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
