// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(FNesTestSax, "Nes.SAX",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesMMU> mmu;
unique_ptr<NesCart> cart;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(FNesTestSax)

void FNesTestSax::Define()
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

	Describe("FNesTestSaxIndirectX", [this]()
	{
		It("A = 0x3E, X = 0x17, P = 0xE6", [this]()
		{
			cart->Write(0, 0x83);
			cart->Write(1, 0x49);
			mmu->AttachCart(move(cart));
			mmu->Write(0x60,0x89);
			mmu->Write(0x61,0x04);
			mmu->Write(0x0489, 0x00);
			CPU->A = 0x3E;
			CPU->X = 0x17;
			CPU->P->PSetState(0xE6);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x3E);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE6);
			TestEqual(TEXT("Memory at 0x0489"), mmu->Read(0x0489), 0x16);
		});
	});
}
