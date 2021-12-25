// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesMMU.h"
#include "NesCart.h"
#include <vector>
#include <memory>

BEGIN_DEFINE_SPEC(FNesTestLd, "Nes.LD",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> Cpu;
shared_ptr<NesMMU> Mmu;
unique_ptr<NesCart> Cart;
uint M_MemorySize = 0x4000;
vector<uint8> Rom;
END_DEFINE_SPEC(FNesTestLd)

void FNesTestLd::Define()
{
	BeforeEach([this]()
	{
		Cpu = make_unique<FNesCPU>();
		Mmu = make_shared<NesMMU>();
		Cpu->AttachMemory(Mmu); //Set PC to 0x8000
		Rom.clear();
		Rom.resize(0x8000, 0);
		Cart = make_unique<NesCart>(Rom);
	});

	Describe("FNesTestLd Immediate", [this]()
	{
		It("LDX at CAB3", [this]()
		{
			Cart->Write(0, 0xA2);
			Cart->Write(1, 0x40);
			Mmu->AttachCart(move(Cart));
			Cpu->X = 0x00;
			Cpu->P->PSetState(0xE5);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 2);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("Y"), Cpu->X, 0x40);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0x65);
		});
	});

	Describe("FNesTestLd", [this]()
	{
		It("LDY at 0x0678 = 0x55 should equal 0x55", [this]()
		{
			Cart->Write(0, 0xAC);
			Cart->Write(1, 0x78);
			Cart->Write(2, 0x06);
			Mmu->Write(0x0678, 0x55);
			Mmu->AttachCart(move(Cart));
			Cpu->Y = 0x00;
			Cpu->P->PSetState(0x67);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8003);
			TestEqual(TEXT("Y"), Cpu->Y, 0x55);
			TestEqual(TEXT("Memory at 0x0678"), Mmu->Read(0x0678), 0x55);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0x65);
		});
	});

	Describe("FNesTestLdZeroPage", [this]()
	{
		It("LDA at 0x0000 = 0x00 should equal 0x00", [this]()
		{
			Cart->Write(0, 0xA5);
			Cart->Write(1, 0x00);
			Mmu->Write(0x0000, 0x00);
			Mmu->AttachCart(move(Cart));
			Cpu->A = 0xA3;
			Cpu->P->PSetState(0x67);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 3);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("A"), Cpu->A, 0x00);
			TestEqual(TEXT("Memory at 0x0000"), Mmu->Read(0x0000), 0x00);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestLdZeroPageX", [this]()
	{
		It("LDY at 0x00FF = 0xBB Y should equal 0xBB", [this]()
		{
			Cart->Write(0, 0xB4);
			Cart->Write(1, 0xFF);
			Mmu->Write(0x0089, 0xBB);
			Mmu->AttachCart(move(Cart));
			Cpu->X = 0x8A;
			Cpu->Y = 0x00;
			Cpu->P->PSetState(0x26);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("X"), Cpu->X, 0x8A);
			TestEqual(TEXT("Y"), Cpu->Y, 0xBB);
			TestEqual(TEXT("Memory at 0x0089"), Mmu->Read(0x0089), 0xBB);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0xA4);
		});
	});

	Describe("FNesTestLdAbsoluteY", [this]()
	{
		It("LDA at 0x0033 = 0xA3 should equal 0xA3", [this]()
		{
			Cart->Write(0, 0xB9);
			Cart->Write(1, 0xFF);
			Cart->Write(2, 0xFF);
			Mmu->Write(0x0033, 0xA3);
			Mmu->AttachCart(move(Cart));
			Cpu->A = 0xFF;
			Cpu->Y = 0x34;
			Cpu->P->PSetState(0x65);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8003);
			TestEqual(TEXT("A"), Cpu->A, 0xA3);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestLdaIndirectX", [this]()
	{
		It("A = 0x5C P = 0x27", [this]()
		{
			Cart->Write(0, 0xA1);
			Cart->Write(1, 0xFF);
			Mmu->AttachCart(move(Cart));
			Mmu->Write(0xFF,0x00);
			Mmu->Write(0x00,0x04);
			Mmu->Write(0x0400, 0x5D);
			Cpu->A = 0x5C;
			Cpu->X = 0x00;
			Cpu->P->PSetState(0x27);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("A"), Cpu->A, 0x5D);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0x25);
		});
	});

	Describe("FNesTestLdaIndirectY", [this]()
	{
		It("A = 0x89 P = 0x27", [this]()
		{
			Cart->Write(0, 0xB1);
			Cart->Write(1, 0x89);
			Mmu->AttachCart(move(Cart));
			Mmu->Write(0x89,0x00);
			Mmu->Write(0x8A,0x03);
			Mmu->Write(0x0300, 0x89);
			Cpu->A = 0x00;
			Cpu->Y = 0x00;
			Cpu->P->PSetState(0x27);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("A"), Cpu->A, 0x89);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0xA5);
		});
	});

	Describe("FNesTestLdaIndirectYCrossPage", [this]()
	{
		It("A = 0x89 P = 0x65", [this]()
		{
			Cart->Write(0, 0xB1);
			Cart->Write(1, 0x97);
			Mmu->AttachCart(move(Cart));
			Mmu->Write(0x97,0xFF);
			Mmu->Write(0x98,0xFF);
			Mmu->Write(0x0033, 0xA3);
			Cpu->A = 0x00;
			Cpu->Y = 0x34;
			Cpu->P->PSetState(0x65);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("A"), Cpu->A, 0xA3);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestLdaIndirectYCrossPage Wrapped", [this]()
	{
		It("A = 0x12 P = 0x65", [this]()
		{
			Cart->Write(0, 0xB1);
			Cart->Write(1, 0xFF);
			Mmu->AttachCart(move(Cart));
			Mmu->Write(0xFF,0x46);
			Mmu->Write(0x00,0x01);
			Mmu->Write(0x0245, 0x12);
			Cpu->A = 0x01;
			Cpu->Y = 0xFF;
			Cpu->P->PSetState(0xE5);
			const uint8 Cycle = Cpu->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), Cpu->PC, 0x8002);
			TestEqual(TEXT("A"), Cpu->A, 0x12);
			TestEqual(TEXT("P"), Cpu->P->PStateWithBFlag(), 0x65);
		});
	});
}