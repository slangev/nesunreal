// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMain.h"
#include <iomanip>

DEFINE_LOG_CATEGORY(LogNesMain);

// Sets default values for this component's properties
UNesMain::UNesMain()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UNesMain::Log(FString msg) {
	UE_LOG(LogNesMain, Log, TEXT("%s"), *msg);
}


// Called when the game starts
void UNesMain::BeginPlay()
{
	Super::BeginPlay();
	M_Ppu = make_unique<NesPPU>(256, 240, 4);
	M_Mmu = make_shared<NesMMU>();
	M_CPU = make_unique<NesCPU>();
	M_Mmu->AttachCart(make_unique<NesCart>(pathToRom));
	M_CPU->AttachMemory(M_Mmu, 0xC000);
	AActor *a = GetOwner();
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(a->FindComponentByClass(UStaticMeshComponent::StaticClass()));
	if(Mesh)
	{
		UMaterialInstanceDynamic* mat = Mesh->CreateDynamicMaterialInstance(
			0, static_cast<UMaterialInterface*>(nullptr), FName(TEXT("Dynamic Mat")));
		mat->SetTextureParameterValue(FName(TEXT("TextureInput")), M_Ppu->GetScreen());
	}
}




// Called every frame
void UNesMain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	uint CyclesThisUpdate = 0 ; 
    while (CyclesThisUpdate < MAXCYCLES) {
		//gbJoyPad.HandleKeyEvents();
		const uint Cycles = M_CPU->Tick();
		CyclesThisUpdate+=Cycles;
		/*gbGraphic.UpdateGraphics(cycles);
		gbAudio.UpdateAudioTimer(cycles);*/
	}
	M_Ppu->RenderStaticByMatrix();
}

