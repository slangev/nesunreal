// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NesPPUMMU.h"
#include <memory>
#include "NesPixel.h"

/**
 * 
 */
using namespace std;
class PPUCTRL {
	public:
		PPUCTRL(){};
		int baseNameTable = 0x0;         // Base nametable, 2 bits, 0 = $2000, 1 = $2400, 2 = $2800, 3 = $2C00
		bool NMIGenerate = false;        // Generate NMI at start of Vertical Blank?
		bool VRAMInc = false;            // false add 1 per CPU read/write of PPUDATA, true add 32
		bool backgroundTable = false;    // Background pattern table, 0: $0000, 1: $1000
		bool spriteTable = false;        // Pattern table for 8x8 sprites, ignored for 8x16. 0: $0000, 1: $1000
		bool tallSprite = false;         // False: 8x8, True: 8x16
		bool PPUMasterSlave = false;     // This definitely does something with background colors

	void WriteCtrl(uint8 Data) {
		baseNameTable = Data & 0x3;
		VRAMInc = (Data & 0x4) > 0;
		spriteTable = (Data & 0x8) > 0;
		backgroundTable = (Data & 0x10) > 0;
		tallSprite = (Data & 0x20) > 0;
		PPUMasterSlave = (Data & 0x40) > 0;
		NMIGenerate = (Data & 0x80) > 0;
	}
};

class PPUMASK {
	public:
		PPUMASK(){};
		bool greyscale = false;          
		bool showBGLeft = false;         // true: show background in lefmost 8 pixels
		bool showSpriteLeft = false;     // ditto but for sprites
		// If the following 2 are disabled, nothing will be rendered
		bool showBG = false;             // Self explanatory
		bool showSprites = false;        // ditto
		// This definitely does something
		bool emphRed = false;            // Emphasize red
		bool emphGreen = false;          // Emphasize green
		bool emphBlue = false;           // Emphasize blue
		void WriteMask(uint8 Data) {
			greyscale = (Data & 0x1) > 0;
			showBGLeft = (Data & 0x2) > 0;
			showSpriteLeft = (Data & 0x4) > 0;
			showBG = (Data & 0x8) > 0;
			showSprites = (Data & 0x10) > 0;
			emphRed = (Data & 0x20) > 0;
			emphGreen = (Data & 0x40) > 0;
			emphBlue = (Data & 0x80) > 0;
		}
};

class PPUSTATUS {
	public:
		PPUSTATUS(){};
		bool spriteOverflow = false;     // Set if more than 8 sprites on a line.
     	bool spriteZeroHit = false;      // Set if nonzero pixel of bg overlaps with nonzero pixel of sprite
    	bool vBlank = false;             // Set if vblank has started. Cleared with status is read and during pre-render
};

class NESUNREAL_API NesPPU
{
public:
	UPROPERTY()
	UTexture2D* DynamicCanvas;
	
	NesPPU();
	NesPPU(int CanvasWidth,int CanvasHeight, int BytesPerPixel);
	~NesPPU();
	void RenderStaticByMatrix();
	void RenderScreen();
	void SetNMIInterrupt(bool request);
	bool GetNMIInterrupt();
	void Step(uint cycle);
	void AttachPPUMUU(shared_ptr<NesPPUMMU> ppu_mmu);
	void WriteRegister(unsigned short Address, uint8 Data);
	uint8 ReadRegister(unsigned short Address);
	UTexture2D* GetScreen() {return DynamicCanvas;};
	void drawBGScanLine(int x, int y, int screenY);
	void drawSprites(int Scanline);
	void ProcessDMA(int Adderss, uint8 Data);


private:
	// canvas
	int CanvasWidth;
	int CanvasHeight;
	int BytesPerPixel;
	bool NMI = false;
	int latch = 0x00;
	// Loopy's scroll registers
    int loopyV = 0;         // Current VRAM address (15-bit)
    int loopyT = 0;         // Temporary VRAM address (15-bit)
    int loopyX = 0;         // Fine X Scroll (3-bit)
    bool loopyW = false; 	// First or second write toggle (1 bit)
	uint8 OAMADDR = 0x0;

    // Internal stuff
    int lineCount = -1;     // Tracking what line the PPU is apparently on
    int cycleCount = 0;     // Track amount of cycles we've gone through
	int lineNumber = 1;

	// PPUData read buffer
    uint8 PPUDataReadBuffer = 0x00;

	PPUCTRL ppuctrl;
	PPUMASK ppumask;
	PPUSTATUS ppustatus;

	// Frame Buffer
	unique_ptr<vector<unique_ptr<vector<NesPixel>>>> VideoMemory;
	shared_ptr<NesPPUMMU> M_Mmu;

	vector<FColor> palettes = { // RGB Color Pallete for NES
           FColor(0x75, 0x75, 0x75, 0xFF), FColor(0x27, 0x1B, 0x8F, 0xFF), FColor(0x00, 0x00, 0xAB, 0xFF), FColor(0x47, 0x00, 0x9F, 0xFF), FColor(0x8F, 0x00, 0x77, 0xFF), FColor(0xAB, 0x00, 0x13, 0xFF), FColor(0xA7, 0x00, 0x00, 0xFF), FColor(0x7F, 0x0B, 0x00, 0xFF), FColor(0x43, 0x2F, 0x00, 0xFF), FColor(0x00, 0x47, 0x00, 0xFF), FColor(0x00, 0x51, 0x00, 0xFF), FColor(0x00, 0x3F, 0x17, 0xFF),
		   FColor(0x1B, 0x3F, 0x5F, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0xBC, 0xBC, 0xBC, 0xFF), FColor(0x00, 0x73, 0xEF, 0xFF), FColor(0x23, 0x3B, 0xEF, 0xFF), FColor(0x83, 0x00, 0xF3, 0xFF), FColor(0xBF, 0x00, 0xBF, 0xFF), FColor(0xE7, 0x00, 0x5B, 0xFF), FColor(0xDB, 0x2B, 0x00, 0xFF), FColor(0xCB, 0x4F, 0x0F, 0xFF),
		   FColor(0x8B, 0x73, 0x00, 0xFF), FColor(0x00, 0x97, 0x00, 0xFF), FColor(0x00, 0xAB, 0x00, 0xFF), FColor(0x00, 0x93, 0x3B, 0xFF), FColor(0x00, 0x83, 0x8B, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0xFF, 0xFF, 0xFF, 0xFF), FColor(0x3F, 0xBF, 0xFF, 0xFF), FColor(0x5F, 0x97, 0xFF, 0xFF), FColor(0xA7, 0x8B, 0xFD, 0xFF),
		   FColor(0xF7, 0x7B, 0xFF, 0xFF), FColor(0xFF, 0x77, 0xB7, 0xFF), FColor(0xFF, 0x77, 0x63, 0xFF), FColor(0xFF, 0x9B, 0x3B, 0xFF), FColor(0xF3, 0xBF, 0x3F, 0xFF), FColor(0x83, 0xD3, 0x13, 0xFF), FColor(0x4F, 0xDF, 0x4B, 0xFF), FColor(0x58, 0xF8, 0x98, 0xFF), FColor(0x00, 0xEB, 0xDB, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF),
		   FColor(0xFF, 0xFF, 0xFF, 0xFF), FColor(0xAB, 0xE7, 0xFF, 0xFF), FColor(0xC7, 0xD7, 0xFF, 0xFF), FColor(0xD7, 0xCB, 0xFF, 0xFF), FColor(0xFF, 0xC7, 0xFF, 0xFF), FColor(0xFF, 0xC7, 0xDB, 0xFF), FColor(0xFF, 0xBF, 0xB3, 0xFF), FColor(0xFF, 0xDB, 0xAB, 0xFF), FColor(0xFF, 0xE7, 0xA3, 0xFF), FColor(0xE3, 0xFF, 0xA3, 0xFF), FColor(0xAB, 0xF3, 0xBF, 0xFF), FColor(0xB3, 0xFF, 0xCF, 0xFF), 
		   FColor(0x9F, 0xFF, 0xF3, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF), FColor(0x00, 0x00, 0x00, 0xFF)
    };

	void incrementLoopyY();
	void incrementLoopyX();
	void checkSpriteOverflow(int lineCount);
};
