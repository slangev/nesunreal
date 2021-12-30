// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include <memory>
#include <vector>
#include "NesCPU.h"
#include "NesPPU.h"
#include "NesCPUMMU.h"
#include "NesCart.h"

BEGIN_DEFINE_SPEC(NesTestST, "Nes.ST", 
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> mmu;
shared_ptr<NesCart> cart;
shared_ptr<NesPPU> ppu;
uint m_memorySize = 0x4000;
vector<uint8> rom;
END_DEFINE_SPEC(NesTestST)

void NesTestST::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		mmu = make_shared<NesCPUMMU>();
		ppu = make_shared<NesPPU>();
		mmu->AttachPPU(ppu);
		rom.clear();
		rom.resize(0x8000,0);
		cart = make_unique<NesCart>(rom);
		//Writing the PC of 0x8000
		cart->Write(0xFFFD, 0x80);
		cart->Write(0xFFFC, 0x00);
	});	

    Describe("NesZeroPageSTX", [this]()
    {
        It("mmu at 0x0011 should contain value 0x1", [this]()
        {
			cart->Write(0x8000,0x86);
            cart->Write(0x8001,0x11);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
            CPU->X = 0x1;
			const uint8 Cycle = CPU->Tick();
            TestEqual(TEXT("Cycle needs to be 3"), 3, Cycle);
            TestEqual(TEXT("PC needs to be 0x8002"), 0x8002, CPU->PC);
            TestEqual(TEXT("X needs to be 1"), 0x1, CPU->X);
            TestEqual(TEXT("mmu at 0x0011 should contain value 0x1"), 0x1, mmu->Read(0x0011));
        });
    });

	Describe("NesZeroPageSTY", [this]()
    {
        It("mmu at 0x0011 should contain value 0x1", [this]()
        {
			cart->Write(0x8000,0x84);
            cart->Write(0x8001,0x11);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
            CPU->Y = 0x1;
			const uint8 Cycle = CPU->Tick();
            TestEqual(TEXT("Cycle needs to be 3"), 3, Cycle);
            TestEqual(TEXT("PC needs to be 0x8002"), 0x8002, CPU->PC);
            TestEqual(TEXT("Y needs to be 1"), 0x1, CPU->Y);
            TestEqual(TEXT("mmu at 0x0011 should contain value 0x1"), 0x1, mmu->Read(0x0011));
        });
    });

	Describe("NesAbsoluteSTY", [this]()
	{
		It("mmu at 0x0678 should contain value 0x46 from Y=0x46", [this]()
		{
			cart->Write(0x8000, 0x8C);
			cart->Write(0x8001, 0x78);
			cart->Write(0x8002, 0x06);
			mmu->Write(0x0678, 0x55);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			TestEqual(TEXT("mmu at 0x11 should be 0x0."), mmu->Read(0x11), 0x0);
			CPU->Y = 0x46;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("Y"), CPU->Y, 0x46);
			TestEqual(TEXT("P"), CPU->P->PState(), 0xE5);
			TestEqual(TEXT("mmu at 0x0678"), mmu->Read(0x0678), CPU->Y);
		});
	});

	Describe("NesAbsoluteSTX", [this]()
    {
        It("mmu at 0x07FF should contain value 0xFB.", [this]()
        {
			cart->Write(0x8000, 0x8E);
            cart->Write(0x8001, 0xFF);
            cart->Write(0x8002, 0x07);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

            TestEqual(TEXT(""),0x0,mmu->Read(0x07FF));
            CPU->X = 0xFB;
			const uint8 Cycle = CPU->Tick();
            TestEqual(TEXT(""),4, Cycle);
            TestEqual(TEXT(""),0x8003, CPU->PC);
            TestEqual(TEXT(""),0xFB,CPU->X);
            TestEqual(TEXT(""),CPU->X,mmu->Read(0x07FF));
        });
    });

	Describe("FNesTestSTAIndirectY", [this]()
	{
		It("A = 0x87 P = 0xE5", [this]()
		{
			cart->Write(0x8000, 0x91);
			cart->Write(0x8001, 0x33);
			mmu->AttachCart(cart);
			mmu->Write(0x33,0x00);
			mmu->Write(0x34,0x04);
			mmu->Write(0x0400, 0x7F);
			CPU->AttachMemory(mmu);

			CPU->A = 0x87;
			CPU->Y = 0x00;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x87);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
			TestEqual(TEXT("mmu at 0x0400"),CPU->A,mmu->Read(0x0400));
		});
	});

	Describe("FNesTestSTAAbsoluteY", [this]()
	{
		It("STA at 0x0033 = 0xAA should equal 0xFF", [this]()
		{
			cart->Write(0x8000, 0x99);
			cart->Write(0x8001, 0xFF);
			cart->Write(0x8002, 0xFF);
			mmu->Write(0x0033, 0x7F);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->A = 0x87;
			CPU->Y = 0x34;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0x87);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
			TestEqual(TEXT("mmu at 0x0033"),CPU->A,mmu->Read(0x0033));
		});
	});

	Describe("FNesTestSTAZeroPageX", [this]()
	{
		It("STY at 0x0033 = 0xAA should equal 0xFF", [this]()
		{
			cart->Write(0x8000, 0x94);
			cart->Write(0x8001, 0xFF);
			mmu->Write(0x0033, 0x7F);
			mmu->AttachCart(cart);
			CPU->AttachMemory(mmu);

			CPU->Y = 0x87;
			CPU->X = 0x34;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->Y, 0x87);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
			TestEqual(TEXT("mmu at 0x0033"),CPU->Y,mmu->Read(0x0033));
		});
	});
}