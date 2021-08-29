// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestAdc, "Nes.ADC",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestAdc)

void FNesTestAdc::Define()
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

	Describe("FNesTestADCAbsolute", [this]()
	{
		It("A = 0x5F P = 64", [this]()
		{
			cart->Write(0, 0x6D);
			cart->Write(1, 0x78);
			cart->Write(2, 0x06);
			mmu->Write(0x0678, 0x69);
			mmu->AttachCart(move(cart));
			CPU->A = 0x00;
			CPU->P->PSetState(0x66);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x69);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});

	Describe("FNesTestADCIndirectY", [this]()
	{
		It("A = 0x71 P = 0x66", [this]()
		{
			cart->Write(0, 0x71);
			cart->Write(1, 0x33);
			mmu->AttachCart(move(cart));
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0x69);
			CPU->A = 0x00;
			CPU->Y = 0x00;
			CPU->P->PSetState(0x66);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x69);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});

	Describe("FNesTestAdcAbsoluteY", [this]()
	{
		It("Adc Y at 0x0033 = 0x69 should equal 0x69", [this]()
		{
			cart->Write(0, 0x79);
			cart->Write(1, 0xFF);
			cart->Write(2, 0xFF);
			mmu->Write(0x0033, 0x69);
			mmu->AttachCart(move(cart));
			CPU->A = 0x00;
			CPU->Y = 0x34;
			CPU->P->PSetState(0x66);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x69);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});

	Describe("FNesTestAdcZeroPageX", [this]()
	{
		It("A = 0x00 P = 66", [this]()
		{
			cart->Write(0, 0x75);
			cart->Write(1, 0x00);
			mmu->Write(0x78, 0x69);
			mmu->AttachCart(move(cart));
			CPU->A = 0x00;
			CPU->X = 0x78;
			CPU->P->PSetState(0x64);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x69);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x24);
		});
	});
}
