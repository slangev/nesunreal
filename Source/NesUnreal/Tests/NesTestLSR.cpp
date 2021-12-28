// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesPPU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestLsr, "Nes.LSR",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestLsr)

void FNesTestLsr::Define()
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

	Describe("FNesTestLsr", [this]()
	{
		It("Lsr at 0x78 = 1 should equal 0", [this]()
		{
			cart->Write(0x8000, 0x46);
			cart->Write(0x8001, 0x78);
			mmu->Write(0x78, 0x01);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x01;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("Memory at 0x78"), mmu->Read(0x78), 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestLsrAbsolute", [this]()
	{
		It("Lsr at 0x0678 = 1 should equal 0", [this]()
		{
			cart->Write(0x8000, 0x4E);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x01);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x01;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("Memory at 0x0678"), mmu->Read(0x0678), 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestLsrAbsoluteX", [this]()
	{
		It("Lsr at 0x0678 = 1 should equal 0", [this]()
		{
			cart->Write(0x8000, 0x5E);
			cart->Write(0x8001, 0x00);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0655, 0x01);
			TestEqual(TEXT("Memory at 0x0655"), mmu->Read(0x0655), 0x01);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x01;
			CPU->X = 0x55;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 7);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x01);
			TestEqual(TEXT("Memory at 0x0655"), mmu->Read(0x0655), 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});
}
