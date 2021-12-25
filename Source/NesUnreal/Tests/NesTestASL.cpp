// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestAsl, "Nes.ASL",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestAsl)

void FNesTestAsl::Define()
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

	Describe("FNesTestAsl", [this]()
	{
		It("Asl at 0x78 = 0x80 should equal 0", [this]()
		{
			cart->Write(0, 0x06);
			cart->Write(1, 0x78);
			mmu->Write(0x78, 0x80);
			mmu->AttachCart(move(cart));
			CPU->A = 0x80;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x80);
			TestEqual(TEXT("Memory at 0x78"), mmu->Read(0x78), 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestAslAbsolute", [this]()
	{
		It("A = 0x80 P = 0xE5", [this]()
		{
			cart->Write(0, 0x0E);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x80);
			mmu->AttachCart(move(cart));
			CPU->A = 0x80;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x80);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0x00);
		});
	});
}
