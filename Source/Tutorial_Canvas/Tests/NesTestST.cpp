// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(NesTestST, "Nes.ST", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
	unique_ptr<NesCPU> cpu;
	shared_ptr<NesMMU> mmu;
	unique_ptr<NesCart> cart;
	uint m_memorySize = 0x4000;
	vector<uint8> rom;
END_DEFINE_SPEC(NesTestST)

void NesTestST::Define()
{
	BeforeEach([this]()
	{
		cpu = make_unique<NesCPU>();
		mmu = make_shared<NesMMU>();
		cpu->AttachMemory(mmu,0x8000); //Set PC to 0x8000
		rom.clear();
		rom.resize(0x8000,0);
		cart = make_unique<NesCart>(rom);
	});	

    Describe("NesZeroPageSTX", [this]()
    {
        It("mmu at 0x0011 should contain value 0x1", [this]()
        {
			cart->Write(0,0x86);
            cart->Write(1,0x11);
			mmu->AttachCart(move(cart));
			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
            cpu->X = 0x1;
			const uint8 Cycle = cpu->Tick();
            TestEqual(TEXT("Cycle needs to be 3"), 3, Cycle);
            TestEqual(TEXT("PC needs to be 0x8002"), 0x8002, cpu->PC);
            TestEqual(TEXT("X needs to be 1"), 0x1, cpu->X);
            TestEqual(TEXT("mmu at 0x0011 should contain value 0x1"), 0x1, mmu->Read(0x0011));
        });
    });

	Describe("NesZeroPageSTY", [this]()
    {
        It("mmu at 0x0011 should contain value 0x1", [this]()
        {
			cart->Write(0,0x84);
            cart->Write(1,0x11);
			mmu->AttachCart(move(cart));
			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
            cpu->Y = 0x1;
			const uint8 Cycle = cpu->Tick();
            TestEqual(TEXT("Cycle needs to be 3"), 3, Cycle);
            TestEqual(TEXT("PC needs to be 0x8002"), 0x8002, cpu->PC);
            TestEqual(TEXT("Y needs to be 1"), 0x1, cpu->Y);
            TestEqual(TEXT("mmu at 0x0011 should contain value 0x1"), 0x1, mmu->Read(0x0011));
        });
    });

	Describe("NesAbsoluteSTY", [this]()
	{
		It("mmu at 0x0678 should contain value 0x46 from Y=0x46", [this]()
		{
			cart->Write(0,0x8C);
			cart->Write(1,0x78);
			cart->Write(2,0x06);
			mmu->Write(0x0678,0x55);
			mmu->AttachCart(move(cart));
			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
			cpu->Y = 0x46;
			cpu->P->pSetState(0xE5);
			const uint8 Cycle = cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), cpu->PC, 0x8003);
			TestEqual(TEXT("Y"), cpu->Y, 0x46);
			TestEqual(TEXT("P"), cpu->P->pState(), 0xE5);
			TestEqual(TEXT("mmu at 0x0678"), mmu->Read(0x0678), cpu->Y);
		});
	});

	Describe("NesAbsoluteSTX", [this]()
    {
        It("mmu at 0x07FF should contain value 0xFB.", [this]()
        {
			cart->Write(0,0x8E);
            cart->Write(1,0xFF);
            cart->Write(2,0x07);
			mmu->AttachCart(move(cart));
            TestEqual(TEXT(""),0x0,mmu->Read(0x07FF));
            cpu->X = 0xFB;
			const uint8 Cycle = cpu->Tick();
            TestEqual(TEXT(""),4, Cycle);
            TestEqual(TEXT(""),0x8003, cpu->PC);
            TestEqual(TEXT(""),0xFB,cpu->X);
            TestEqual(TEXT(""),cpu->X,mmu->Read(0x07FF));
        });
    });
}