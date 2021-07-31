// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestEOR, "Nes.EOR",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<NesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestEOR)

void FNesTestEOR::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<NesCPU>();
		mmu = make_shared<NesMMU>();
		CPU->AttachMemory(mmu, 0x8000); //Set PC to 0x8000
		rom.clear();
		rom.resize(0x8000, 0);
		cart = make_unique<NesCart>(rom);
	});

	Describe("FNesTestEOR", [this]()
	{
		It("A = 0xF5 P = E4", [this]()
		{
			cart->Write(0, 0x45);
			cart->Write(1, 0x78);
			mmu->Write(0x78, 0xAA);
			mmu->AttachCart(move(cart));
			CPU->A = 0x5F;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle needs to be 3"), Cycle, 3);
			TestEqual(TEXT("PC needs to be 0x8002"), CPU->PC, 0x8002);
			TestEqual(TEXT("A needs to be 0xF5"), CPU->A, 0xF5);
			TestEqual(TEXT("P needs to be 0xE4"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});

	Describe("FNesTestEORAbsolute", [this]()
	{
		It("A = 0x5F P = 64", [this]()
		{
			cart->Write(0, 0x4D);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0xAA);
			mmu->AttachCart(move(cart));
			CPU->A = 0x5F;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0xF5);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});

	Describe("FNesTestEORIndirectY", [this]()
	{
		It("A = 0x5F P = 0x64", [this]()
		{
			cart->Write(0, 0x51);
			cart->Write(1, 0x33);
			mmu->AttachCart(move(cart));
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0xAA);
			CPU->A = 0x5F;
			CPU->Y = 0x00;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0xF5);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});

	Describe("FNesTestEorAbsoluteY", [this]()
	{
		It("EOR Y at 0x0033 = 0xAA should equal 0xF5", [this]()
		{
			cart->Write(0, 0x59);
			cart->Write(1, 0xFF);
			cart->Write(2, 0xFF);
			mmu->Write(0x0033, 0xAA);
			mmu->AttachCart(move(cart));
			CPU->A = 0x5F;
			CPU->Y = 0x34;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0xF5);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});

	Describe("FNesTestEorZeroPageX", [this]()
	{
		It("A = 0xFF P = E4", [this]()
		{
			cart->Write(0, 0x55);
			cart->Write(1, 0x00);
			mmu->Write(0x78, 0xAA);
			mmu->AttachCart(move(cart));
			CPU->A = 0x5F;
			CPU->X = 0x78;
			CPU->P->pSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0xF5);
			TestEqual(TEXT("P"), CPU->P->pStateWithBFlag(), 0xE4);
		});
	});
}
