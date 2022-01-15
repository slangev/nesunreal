// Fill out your copyright notice in the Description page of Project Settings.


#include "NesPPU.h"

DEFINE_LOG_CATEGORY_STATIC(LogNesPPU,Log,All)


NesPPU::NesPPU()
{
    //dynamic texture initialization
	CanvasWidth = 256;
	CanvasHeight = 240;

	DynamicCanvas = UTexture2D::CreateTransient(CanvasWidth, CanvasHeight);
	DynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicCanvas->SRGB = false;
	DynamicCanvas->AddToRoot();
	DynamicCanvas->Filter = TextureFilter::TF_Nearest;
	DynamicCanvas->UpdateResource();

	VideoMemory = make_unique<vector<unique_ptr<vector<NesPixel>>>>();
	for(int i = 0; i < CanvasWidth; i++)
	{
		VideoMemory->push_back(make_unique<vector<NesPixel>>(CanvasHeight));
	}
	
	FColor red;
	red.A = 255;
	red.R = 255;
	red.G = 0;
	red.B = 0;
	
	VideoMemory->at(0)->at(0).pixel = red;
	// buffers initialization
	BytesPerPixel = 4; // r g b a
}

NesPPU::NesPPU(int CanvasWidth, int CanvasHeight, int BytesPerPixel) {
    this->CanvasWidth = CanvasWidth;
    this->CanvasHeight = CanvasHeight;
    this->BytesPerPixel = BytesPerPixel;
	DynamicCanvas = UTexture2D::CreateTransient(CanvasWidth, CanvasHeight);
	DynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	DynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicCanvas->SRGB = true;
	DynamicCanvas->AddToRoot();
	DynamicCanvas->Filter = TextureFilter::TF_Nearest;
	DynamicCanvas->UpdateResource();
	VideoMemory = make_unique<vector<unique_ptr<vector<NesPixel>>>>();
    for(int i = 0; i < CanvasWidth; i++)
    {
    	VideoMemory->push_back(make_unique<vector<NesPixel>>(CanvasHeight));
    }
    
	for(auto &Outer : *VideoMemory)
	{
		for(auto &Inner : *Outer)
		{
			Inner.pixel = FColor::White;
		}
	}
}

NesPPU::~NesPPU() 
{

}

void NesPPU::ProcessDMA(int Address, uint8 Data) {
	int DMAAdress = (OAMADDR + Address) & 0xFF;
    M_Mmu->oam->at(DMAAdress) = Data;
}

uint8 PPURegGetBit(const uint8 Pos, const uint8 Reg) {
    uint8 Result = static_cast<uint8>((Reg & (1 << Pos)));
    Result = static_cast<uint8>(Result >> Pos); 
    return Result;
}

uint8 PPURegSetBit(const uint8 Pos, const uint8 Reg) {
    const uint8 Result = static_cast<uint8>(Reg | (1 << Pos));
    return Result;
}

uint8 PPURegResetBit(const uint8 Pos, const uint8 Reg) {
    const uint8 Result = static_cast<uint8>((Reg & ~(1 << Pos)));
    return Result;
}

//https://wiki.nesdev.org/w/index.php/PPU_scrolling#.242000_write
void NesPPU::WriteRegister(unsigned short Address, uint8 Data) {
	//$2008-$3FFF Mirrors of $2000-2007 (repeats every 8 bytes) 
	latch = Data;
	if(Address == 0x2000) {
		ppuctrl.WriteCtrl(Data);

		// NMI triggered on register write if vBlank is in progress during this.
		// Apparently activation here is dependent on Vblank flag that can be disabled by reading $2002
		if(ppuctrl.NMIGenerate && ppustatus.vBlank){
			NMI = true;
		}
		else if (!ppuctrl.NMIGenerate){
			// NMI can be regenerated by toggling the NMIGenerate flag, so we set it to false to allow that.
			NMI = false;
		}
		// Loopy write
		loopyT = (loopyT & 0xF3FF)|(ppuctrl.baseNameTable << 10);
	} else if(Address == 0x2001) {
		ppumask.WriteMask(Data);
	} else if(Address == 0x2003) {
		OAMADDR = Data;
	} else if(Address == 0x2004) {
		M_Mmu->oam->at(OAMADDR++) = Data;
	} else if(Address == 0x2005) {
		//Writing X
		if(!loopyW) {
			int fiveBitsForLoopyT = (Data >> 3);
			loopyT = (loopyT & 0xFFE0) | fiveBitsForLoopyT;
			loopyX = Data & 0x7;
		}
		//Writing Y 
		else {
			int part1 = Data & 0x7;
			int part2 = (Data >> 3) & 0x7;
			int part3 = (Data >> 6) & 0x3;
			loopyT = (loopyT & 0x8C1F)|(part1 << 12)|(part2 << 5)|(part3 << 8);
		}
		loopyW = !loopyW;
	} else if(Address == 0x2006) {
		if(!loopyW) {
			int sixBitsForLoopyT = (Data & 0x3F) << 8;
			loopyT = (loopyT & 0x00FF) | sixBitsForLoopyT;
		} else {
			loopyT = (loopyT & 0xFF00) | (Data & 0xFF);
			loopyV = loopyT;
		}
		loopyW = !loopyW;
	} else if(Address == 0x2007) {
		M_Mmu->Write(loopyV, Data);
		// Loopy Increment
		loopyV = (loopyV + (ppuctrl.VRAMInc ? 32:1)) & 0xFFFF;
	}
	
}

uint8 NesPPU::ReadRegister(unsigned short Address) {
	uint8 data = latch;
	if(Address == 0x2002) {
		data = latch & 0x1F;
		(ppustatus.spriteOverflow) ? data = PPURegSetBit(5,data) : data = PPURegResetBit(5,data);
		(ppustatus.spriteZeroHit) ? data = PPURegSetBit(6,data) : data = PPURegResetBit(6,data);
		(ppustatus.vBlank) ? data =  PPURegSetBit(7,data) : data = PPURegResetBit(7,data);
		ppustatus.vBlank = false;
		loopyW = false;
	} else if(Address == 0x2004) {
		data = M_Mmu->oam->at(OAMADDR); // reads during vertical or forced blanking return the value from OAM at that address but do not increment. 
	} else if(Address == 0x2007) {
		if (loopyV < 0x3F00){
			data = PPUDataReadBuffer;
			PPUDataReadBuffer = M_Mmu->Read(loopyV);
		}
		// Palette data comes directly however
		else {
			data = M_Mmu->Read(loopyV);
			// Buffer contains mirrored nametable byte instead
			PPUDataReadBuffer = M_Mmu->Read(loopyV & 0x2F1F);
		}
		// Loopy Increment
		loopyV = (loopyV + (ppuctrl.VRAMInc ? 32:1)) & 0xFFFF;
	}
	latch = data;
	return data;
}

void NesPPU::AttachPPUMUU(shared_ptr<NesPPUMMU> mmu) {
	this->M_Mmu = mmu;
}

void NesPPU::SetNMIInterrupt(bool request) {
	NMI = request;
}

bool NesPPU::GetNMIInterrupt() {
	return NMI;
}

void NesPPU::RenderScreen()
{
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormattedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );
	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor Pixel = VideoMemory->at(X)->at(Y).pixel;
			FormattedImageData[Y * MyMipMap->SizeX + X] = Pixel;
		}
	}

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}

void NesPPU::RenderStaticByMatrix() {
	FTexture2DMipMap* MyMipMap = &DynamicCanvas->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormattedImageData = static_cast<FColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );

	for(int32 X = 0; X < MyMipMap->SizeX; X++)
	{
		for (int32 Y = 0; Y < MyMipMap->SizeY; Y++)
		{
			FColor pixel;
			if(FMath::RandRange(0,2) == 0) {
				 pixel.A = 255;
				 pixel.R = 255;
				 pixel.G = 255;
				 pixel.B = 255;
			} else {
				 pixel.A = 255;
				 pixel.R = 0;
				 pixel.G = 0;
				 pixel.B = 0;
			}
			FormattedImageData[Y * MyMipMap->SizeX + X] = pixel;
		}
	}

	DynamicCanvas->PlatformData->Mips[0].BulkData.Unlock();
	DynamicCanvas->UpdateResource();
}

void NesPPU::incrementLoopyY() {
	// Loopy increment Y
	// From the NESDev psuedocode
	if ((loopyV & 0x7000) != 0x7000){
		loopyV = (loopyV + 0x1000) & 0xFFFF;
	}
	else{
		loopyV = (loopyV & ~0x7000) & 0xFFFF;
		int y = (loopyV & 0x03E0) >> 5;
		if (y == 29){
			y = 0;
			loopyV = (loopyV ^ 0x800) & 0xFFFF;
		}
		else if (y == 31){
			y = 0;
		}
		else{
			y += 1;
		}
		loopyV = ((loopyV & ~0x03E0) & 0xFFFF)| (y << 5);
	}
}

 void NesPPU::incrementLoopyX() {
	// Coarse X Increment from nesdev psuedocode
	if ((loopyV & 0x001F) == 31){
		loopyV = (loopyV & ~0x001F) & 0xFFFF;
		loopyV = (loopyV ^ 0x0400) & 0xFFFF;
	}
	else{
		loopyV = (loopyV + 1) & 0xFFFF;
	}
}

void NesPPU::drawSprites(int scanline){
	bool use8x16 = ppuctrl.tallSprite;
	unsigned short patternTable = (ppuctrl.spriteTable) ? 0x1000 : 0x0000;
	int ysize = (use8x16) ? 16 : 8;
	uint8 LY = scanline;
	int spritecount = 0;
	for(int i = 0; i < 64 && spritecount < 9; i++) {
		int PosY = M_Mmu->oam->at(i*4) + 1;
		uint8 tileID = M_Mmu->oam->at(i*4 + 1);
		uint8 attributes = M_Mmu->oam->at(i*4 + 2);
		int PosX = M_Mmu->oam->at(i*4 + 3);
		if((LY >= PosY) && (LY < (PosY+ysize))) {
            spritecount++;
			uint8 spritePriorityBit = PPURegGetBit(5,attributes);
			uint8 xFlipBit = PPURegGetBit(6,attributes);
			uint8 yFlipBit = PPURegGetBit(7,attributes);
			int line = LY - PosY;
			if(yFlipBit == 1) {
				line = ysize - line - 1;
			}
			unsigned short tileLocation = patternTable + (tileID * 16) + (line);
			// Read two bytes of data. These bytes determine the color of the pixel
			uint8 Data1 = M_Mmu->Read(tileLocation);
			uint8 Data2 = M_Mmu->Read(tileLocation + 8);
			 for (int tilePixel = 7; tilePixel >= 0; tilePixel--) {
				int colorBit = tilePixel;
				if (xFlipBit == 1) {
					colorBit -= 7;
					colorBit *= -1;
				}

				uint8 bitFromData1 = PPURegGetBit(colorBit,Data1);
				uint8 bitFromData2 = PPURegGetBit(colorBit,Data2);
				uint8 colorNum = 0;
				if(bitFromData1 == 1) {
					colorNum = PPURegSetBit(0,colorNum);
				}
				if(bitFromData2 == 1) {
					colorNum = PPURegSetBit(1,colorNum);
				}
				// If the pixel is 0 before template is applied, ignore it.
				if(colorNum == 0) {
					continue;
				}
				uint8 msbits = attributes & 0x3;
				int paletteIndex = (msbits << 2) | colorNum;
				int xPix = 0 - tilePixel;
				xPix += 7;
				uint8 pixelPos = (uint8)(PosX+xPix);
				if(i == 0 && ppumask.showBG) {
					ppustatus.spriteZeroHit = true;
				} 
				if ((LY<0)||(LY>239)||(pixelPos<0)||(pixelPos>255)) {
					UE_LOG(LogNesPPU, Warning, TEXT("LY: %d pixelPos: %d"), LY, pixelPos);
					continue;
				}
				//0x3F10 is start of Sprite palette
				VideoMemory->at(pixelPos)->at(LY).pixel = palettes.at(M_Mmu->Read(0x3F10 + paletteIndex) & 0x3F);
			 }
		}
	}
	if(spritecount >= 9) {
		ppustatus.spriteOverflow = true;
	}
	ysize = spritecount;
}

void NesPPU::drawBGScanLine(int x, int y, int screenY) {
	int baseX = x;
	int baseY = y;
	int patternTable = ppuctrl.backgroundTable ? 0x1000:0x0000;
	for (int i = 0; i < 256; i++){
		int drawX = (baseX + i) & 0x1FF;
		int drawY = baseY % 480;

		// Determine nametable region
		int baseNameTable2 = 0x2000;
		if (drawX >= 256){
			baseNameTable2 += 0x400;
			drawX -= 256;
		}
		if (drawY >= 240){
			baseNameTable2 += 0x800;
			drawY -= 240;
		}
		// Determine the tile to render from
		int tileNum = (drawX/8) + ((drawY/8)*32);
		// Determine which pixel of that tile to render from
		int pixelX = drawX % 8;
		int pixelY = drawY % 8;

		// Get pixel data (specifically the palette data)
		int tilePointer = M_Mmu->Read(baseNameTable2 + tileNum); // Tile pointer
		int tileX = tileNum % 32;
		int tileY = tileNum / 32;
		int attributeByte = (baseNameTable2 + 0x3C0) + ((tileX/4) + ((tileY/4) * 8));
		int cornerNum = (((tileX/2)%2) + (((tileY/2)%2)*2))*2;
		int attribute = (M_Mmu->Read(attributeByte) >> cornerNum) & 0x3;

		int paletteIndex = (attribute << 2) | ((M_Mmu->Read(patternTable + (tilePointer << 4) + pixelY) >> (7 - pixelX)) & 0x1)|
				((M_Mmu->Read(patternTable + (tilePointer << 4) + (pixelY + 8)) >> (7 - pixelX)) & 0x1) << 1;

		FColor pixelColor;
		if ((paletteIndex & 0x3) != 0 && !(!ppumask.showBGLeft && i < 8)){
			pixelColor = palettes.at(M_Mmu->Read(0x3F00 + paletteIndex) & 0x3F);
			VideoMemory->at(i)->at(lineCount).pixel = pixelColor;
		} 
		else {
			pixelColor = palettes.at(M_Mmu->Read(0x3F00 + paletteIndex) & 0x3F);
			VideoMemory->at(i)->at(lineCount).pixel = pixelColor; //Transparent?
		}
	}
}

void NesPPU::Step(uint Cycle) {
	while(Cycle-- > 0) {
		cycleCount++;

		if (cycleCount == 256){
			if ((ppumask.showBG || ppumask.showSprites) && lineCount < 240) {
				incrementLoopyY();
			}
		}
		else if (cycleCount == 257){
			// Copy horizontal bits each scanline
			if ((ppumask.showBG || ppumask.showSprites) && lineCount < 240) {
				loopyV = (loopyV & 0xFBE0) | (loopyT & 0x041F);
			}
		}
		else if (cycleCount == 304){
			// Copy vertical bits during pre-render line
			if ((ppumask.showBG || ppumask.showSprites) && lineCount == -1) {
				loopyV = (loopyV & 0x41F)|(loopyT & 0xFBE0);
			}
		}
		//Vblank trigger 
		else if (lineCount == 241 && cycleCount == 1) {
			// Trigger NMI as soon as vblank reached
			ppustatus.vBlank = true;
			NMI = ppuctrl.NMIGenerate;
		} 
		//Reset values
		else if (lineCount == 260 && cycleCount == 1) {
			lineCount = -1;
			ppustatus.vBlank = false;
			ppustatus.spriteZeroHit = false;
			ppustatus.spriteOverflow = false;
			NMI = false;
		}
		else if (cycleCount > 340) {
			cycleCount = 0;
			lineCount++;
			if (lineCount < 240) {
				// Attempt with loopy's data
				if (ppumask.showBG) {
					bool baseNameTableX = (loopyV & 0x400) == 0x400;
					bool baseNameTableY = (loopyV & 0x800) == 0x800;
					int loopyXscroll = (((loopyV & 0x1F) << 3)|loopyX) + ((baseNameTableX) ? 256 : 0);
					int loopyYscroll = (((loopyV >> 12) & 0x7)|(((loopyV >> 5) & 0x7) << 3)|
							(((loopyV >> 8) & 0x3) << 6)) + (baseNameTableY ? 240:0);
					drawBGScanLine(loopyXscroll, loopyYscroll, lineCount);
				}
				if (ppumask.showSprites) {
					drawSprites(lineCount);
				}
			} 
		}
	}
}