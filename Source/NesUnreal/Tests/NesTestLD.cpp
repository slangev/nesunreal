// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/AutomationTest.h"
#include "NesCPU.h"
#include "NesCPUMMU.h"
#include "NesPPU.h"
#include "NesCart.h"
#include <vector>
#include <memory>

BEGIN_DEFINE_SPEC(FNesTestLd, "Nes.LD",
				EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
unique_ptr<FNesCPU> CPU;
shared_ptr<NesCPUMMU> Mmu;
shared_ptr<NesCart> Cart;
shared_ptr<NesPPU> ppu;
uint M_MemorySize = 0x4000;
vector<uint8> Rom;
END_DEFINE_SPEC(FNesTestLd)

void FNesTestLd::Define()
{
	BeforeEach([this]()
	{
		CPU = make_unique<FNesCPU>();
		Mmu = make_shared<NesCPUMMU>();
		ppu = make_shared<NesPPU>();
		Mmu->AttachPPU(ppu);
		Rom.clear();
		Rom.resize(0x8000, 0);
		Cart = make_unique<NesCart>(Rom);
		//Writing the PC of 0x8000
		Cart->Write(0xFFFD, 0x80);
		Cart->Write(0xFFFC, 0x00);
	});

	Describe("FNesTestLd Immediate", [this]()
	{
		It("LDX at CAB3", [this]()
		{
			Cart->Write(0x8000, 0xA2);
			Cart->Write(0x8001, 0x40);
			Mmu->AttachCart(Cart);
			CPU->AttachMemory(Mmu);

			CPU->X = 0x00;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 2);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("Y"), CPU->X, 0x40);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x65);
		});
	});

	Describe("FNesTestLd", [this]()
	{
		It("LDY at 0x0678 = 0x55 should equal 0x55", [this]()
		{
			Cart->Write(0x8000, 0xAC);
			Cart->Write(0x8001, 0x78);
			Cart->Write(0x8002, 0x06);
			Mmu->Write(0x0678, 0x55);
			Mmu->AttachCart(Cart);
			CPU->AttachMemory(Mmu);

			CPU->Y = 0x00;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("Y"), CPU->Y, 0x55);
			TestEqual(TEXT("Memory at 0x0678"), Mmu->Read(0x0678), 0x55);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x65);
		});
	});

	Describe("FNesTestLdZeroPage", [this]()
	{
		It("LDA at 0x0000 = 0x00 should equal 0x00", [this]()
		{
			Cart->Write(0x8000, 0xA5);
			Cart->Write(0x8001, 0x00);
			Mmu->Write(0x0000, 0x00);
			Mmu->AttachCart(Cart);
			CPU->AttachMemory(Mmu);

			CPU->A = 0xA3;
			CPU->P->PSetState(0x67);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 3);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x00);
			TestEqual(TEXT("Memory at 0x0000"), Mmu->Read(0x0000), 0x00);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x67);
		});
	});

	Describe("FNesTestLdZeroPageX", [this]()
	{
		It("LDY at 0x00FF = 0xBB Y should equal 0xBB", [this]()
		{
			Cart->Write(0x8000, 0xB4);
			Cart->Write(0x8001, 0xFF);
			Mmu->Write(0x0089, 0xBB);
			Mmu->AttachCart(Cart);
			CPU->AttachMemory(Mmu);

			CPU->X = 0x8A;
			CPU->Y = 0x00;
			CPU->P->PSetState(0x26);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 4);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("X"), CPU->X, 0x8A);
			TestEqual(TEXT("Y"), CPU->Y, 0xBB);
			TestEqual(TEXT("Memory at 0x0089"), Mmu->Read(0x0089), 0xBB);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xA4);
		});
	});

	Describe("FNesTestLdAbsoluteY", [this]()
	{
		It("LDA at 0x0033 = 0xA3 should equal 0xA3", [this]()
		{
			Cart->Write(0x8000, 0xB9);
			Cart->Write(0x8001, 0xFF);
			Cart->Write(0x8002, 0xFF);
			Mmu->Write(0x0033, 0xA3);
			Mmu->AttachCart(Cart);
			CPU->AttachMemory(Mmu);

			CPU->A = 0xFF;
			CPU->Y = 0x34;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8003);
			TestEqual(TEXT("A"), CPU->A, 0xA3);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestLdaIndirectX", [this]()
	{
		It("A = 0x5C P = 0x27", [this]()
		{
			Cart->Write(0x8000, 0xA1);
			Cart->Write(0x8001, 0xFF);
			Mmu->AttachCart(Cart);
			Mmu->Write(0xFF,0x00);
			Mmu->Write(0x00,0x04);
			Mmu->Write(0x0400, 0x5D);
			CPU->AttachMemory(Mmu);

			CPU->A = 0x5C;
			CPU->X = 0x00;
			CPU->P->PSetState(0x27);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x5D);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x25);
		});
	});

	Describe("FNesTestLdaIndirectY", [this]()
	{
		It("A = 0x89 P = 0x27", [this]()
		{
			Cart->Write(0x8000, 0xB1);
			Cart->Write(0x8001, 0x89);
			Mmu->AttachCart(Cart);
			Mmu->Write(0x89,0x00);
			Mmu->Write(0x8A,0x03);
			Mmu->Write(0x0300, 0x89);
			CPU->AttachMemory(Mmu);

			CPU->A = 0x00;
			CPU->Y = 0x00;
			CPU->P->PSetState(0x27);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 5);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x89);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xA5);
		});
	});

	Describe("FNesTestLdaIndirectYCrossPage", [this]()
	{
		It("A = 0x89 P = 0x65", [this]()
		{
			Cart->Write(0x8000, 0xB1);
			Cart->Write(0x8001, 0x97);
			Mmu->AttachCart(Cart);
			Mmu->Write(0x97,0xFF);
			Mmu->Write(0x98,0xFF);
			Mmu->Write(0x0033, 0xA3);
			CPU->AttachMemory(Mmu);

			CPU->A = 0x00;
			CPU->Y = 0x34;
			CPU->P->PSetState(0x65);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0xA3);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0xE5);
		});
	});

	Describe("FNesTestLdaIndirectYCrossPage Wrapped", [this]()
	{
		It("A = 0x12 P = 0x65", [this]()
		{
			Cart->Write(0x8000, 0xB1);
			Cart->Write(0x8001, 0xFF);
			Mmu->AttachCart(Cart);
			Mmu->Write(0xFF,0x46);
			Mmu->Write(0x00,0x01);
			Mmu->Write(0x0245, 0x12);
			CPU->AttachMemory(Mmu);

			CPU->A = 0x01;
			CPU->Y = 0xFF;
			CPU->P->PSetState(0xE5);
			const uint8 Cycle = CPU->Tick();
			TestEqual(TEXT("Cycle"), Cycle, 6);
			TestEqual(TEXT("PC"), CPU->PC, 0x8002);
			TestEqual(TEXT("A"), CPU->A, 0x12);
			TestEqual(TEXT("P"), CPU->P->PStateWithBFlag(), 0x65);
		});
	});
}