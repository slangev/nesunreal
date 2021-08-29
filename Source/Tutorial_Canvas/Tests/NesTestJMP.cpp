// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestJmp, "Nes.JMP",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestJmp)

void FNesTestJmp::Define()
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

	Describe("FNesTestJMPAbsolute", [this]()
	{
		It("A = 0x00 P = 0x24 PC = 0xC5F5", [this]()
		{
			cart->Write(0, 0x4C);
			cart->Write(1, 0xF5);
			cart->Write(2, 0xC5);
			mmu->AttachCart(move(cart));
			CPU->A = 0x00;
			CPU->P->PSetState(0x24);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 3);
			TestEqual(TEXT("PC"), CPU->PC, 0xC5F5);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});

	Describe("FNesTestJMPIndirect", [this]()
	{
		It("A = 0xDB P = 0xE5 PC = 0xDB7E", [this]()
		{
			cart->Write(0, 0x6C);
			cart->Write(1, 0x00);
			cart->Write(2, 0x02);
			mmu->AttachCart(move(cart));
			mmu->Write(0x0200,0x7E);
			mmu->Write(0x0201,0xDB);
			CPU->A = 0xDB;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0xDB7E);
			TestEqual(TEXT("A"), CPU->A, 0xDB);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});
}
