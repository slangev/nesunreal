// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesPPU.h"
#include "NesCart.h"
#include <vector>
#include <memory>

BEGIN_DEFINE_SPEC(FNesTestLax, "Nes.LAX",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestLax)

void FNesTestLax::Define()
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

	Describe("FNesTestLax Indirect X", [this]()
	{
		It("LAX A/X = 0x55 ", [this]()
		{
			cart->Write(0x8000, 0xA3);
			cart->Write(0x8001, 0x40);
			mmu->AttachCart(cart);
			mmu->Write(0x0580, 0x55);
			mmu->Write(0x43,0x80);
			mmu->Write(0x44,0x05);
			CPU->AttachMemory(mmu);

			CPU->X = 0x03;
			CPU->A = 0x00;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("X"), CPU->X, 0x55);
			TestEqual(TEXT("A"), CPU->A, 0x55);
			TestEqual(TEXT("Memory at 0x0580"), mmu->Read(0x0580), 0x55);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x65);
		});
	});

	Describe("FNesTestLax Zero Page", [this]()
	{
		It("LAX A/X = 0x55 ", [this]()
		{
			cart->Write(0x8000, 0xA7);
			cart->Write(0x8001, 0x67);
			mmu->AttachCart(cart);
			mmu->Write(0x0067, 0x87);
			CPU->AttachMemory(mmu);

			CPU->X = 0xAA;
			CPU->A = 0x00;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 3);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("X"), CPU->X, 0x87);
			TestEqual(TEXT("A"), CPU->A, 0x87);
			TestEqual(TEXT("Memory at 0x0580"), mmu->Read(0x0067), 0x87);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestLax Absolute", [this]()
	{
		It("LAX A/X = 087 ", [this]()
		{
			cart->Write(0x8000, 0xAF);
			cart->Write(0x8001, 0x77);
			cart->Write(0x8002, 0x05);
			mmu->AttachCart(cart);
			mmu->Write(0x0577, 0x87);
			CPU->AttachMemory(mmu);

			CPU->X = 0x32;
			CPU->A = 0x00;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("X"), CPU->X, 0x87);
			TestEqual(TEXT("A"), CPU->A, 0x87);
			TestEqual(TEXT("Memory at 0x0577"), mmu->Read(0x0577), 0x87);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});
}